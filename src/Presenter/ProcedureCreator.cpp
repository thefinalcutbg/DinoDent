#include "ProcedureCreator.h"
#include <set>
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"

using namespace Dental;

ProcedureCreator::ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth)
	: m_selectedTeeth(selectedTeeth)
{}

void ProcedureCreator::setProcedureCode(const ProcedureCode& code, bool nhif, double value)
{
	view->setCurrentPresenter(this);

	m_code = code;

	if (!m_code.isValid()) {
		view->setErrorMsg("Изберете процедура");
		return;
	}

	auto scope = code.getScope();

	if ((scope == ProcedureScope::SingleTooth || scope == ProcedureScope::Ambi) 
		&& m_selectedTeeth.empty()) 
	{
		view->setErrorMsg("За да добавите тази процедура изберете поне един зъб");
		return;
	}

	IProcedureInput::Data data;

	data.code = code;

	auto& defaultICDCode = code.defaultICD10();

	if (defaultICDCode.size()) {
		data.diagnosis.icd = defaultICDCode;
	}
	else {
		data.diagnosis = Diagnosis(diag_map[code.type()], true);
	}

	data.financingSource = nhif ? FinancingSource::NHIF : FinancingSource::None;
	data.hyperdontic = false;
	data.value = value;

	if (code.type() == ProcedureType::Restoration) {

		if (m_selectedTeeth.size()) {
			data.param = autoSurfaces(*m_selectedTeeth[0]);
		}
	}

	if (code.type() == ProcedureType::Anesthesia) {

		data.param = AnesthesiaMinutes{ 180 };
	}

	if (scope == ProcedureScope::Range)
	{
		data.range = getBridgeRange(m_selectedTeeth, m_code);

		if (scope == ProcedureScope::Range && m_code.type() != ProcedureType::Denture) {
			data.value = data.value * data.range->getTeethCount();
		}
	}

	if (scope == ProcedureScope::Ambi) {

		if (code.type() != ProcedureType::Crown && m_selectedTeeth.size() > 1) {
			data.range = getBridgeRange(m_selectedTeeth, m_code);
			data.value = data.value * data.range->getTeethCount();
		}

		
	}

	view->setData(data);


}

std::vector<Procedure> ProcedureCreator::getProcedures()
{
	std::vector<Procedure> result;

	if (!m_code.isValid()) return result;

	auto err = view->isValid();

	if (err.size()) {
		ModalDialogBuilder::showMessage(err);
		return {};
	}

	Procedure procedure;

	procedure.code = m_code;
	procedure.date = view->dateEdit()->getDate();

	auto data = view->getData();

	procedure.financingSource = data.financingSource;
	procedure.LPK = User::doctor().LPK;
	procedure.diagnosis = data.diagnosis;
	procedure.notes = data.notes;
	procedure.param = data.param;
	procedure.value = data.value;

	if(data.range.has_value()) {
		procedure.affectedTeeth = data.range.value();
	}

	switch (procedure.code.getScope()) 
	{
		case ProcedureScope::AllOrNone: 
			return { procedure };

		case ProcedureScope::Range:
			return { procedure };

		case ProcedureScope::Ambi:
			if (procedure.getScope() == ProcedureScope::Range) {
				return { procedure };
			}
			break;
		default:
			break;
	}

	//return separate procedures

	for (auto t : m_selectedTeeth)
	{
		auto index = t->toothIndex();
		index.supernumeral = data.hyperdontic;
		procedure.affectedTeeth = index;

		//checks if the procedure is restoration and post is enabled
		bool addPostProcedure =
			!m_code.isLegacy()
			&& std::holds_alternative<RestorationData>(procedure.param)
			&& std::get<RestorationData>(procedure.param).post;

		//adding post as separate procedure
		if (addPostProcedure) {

			Procedure postProcedure = procedure;

			//changing the financing source
			if (postProcedure.financingSource == FinancingSource::NHIF) {
				postProcedure.financingSource = FinancingSource::None;
			}

			postProcedure.code = ProcedureCode("97594-01");
			postProcedure.param = std::monostate{};
			
			result.push_back(postProcedure);
		}

		result.push_back(procedure);

		//removing the post flag
		if (addPostProcedure) {
			std::get<RestorationData>(result.back().param).post = false;
		}
	}

	return result;
}




int ProcedureCreator::restorationDiagnosis(const Tooth& tooth)
{
	/*
		bool secondaryCaries = false;

		for (int i = 0; i < 6; i++)		//checking if somewhere restoration is present also, returning secondary caries
		{
			if (tooth.caries.exists(i) && tooth.restoration.exists(i))
			{
				secondaryCaries = true;
			}
		}
	*/
	std::array<bool, 6> existing
	{
			tooth[Fracture],
			tooth[Caries],
			tooth[RootCanal],
			tooth[Pulpitis],
			tooth[ApicalLesion],
			tooth[Root]
	};

	std::array<int, 7> diagnosis
	{
		4,
		1,
		8,
		2,
		3,
		4 //should be root
	};

	for (int i = 0; i < 6; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::extractionDiagnosis(const Tooth& tooth)
{
	std::array<bool, 7> existing
	{
		tooth[ApicalLesion],
		tooth[Temporary],
		tooth[Root],
		tooth[Periodontitis],
		tooth[Mobility],
		tooth[Fracture],
		tooth[Pulpitis]
	};

	std::array<int, 7> diagnosis
	{
		3,
		7,	//should be deciduous
		4,	//should be root
		7,
		7,
		4,
		2
	};

	for (int i = 0; i < 7; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::endodonticDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth[Pulpitis],
		tooth[ApicalLesion],
		tooth[RootCanal],
		tooth[Fracture]
	};

	std::array<int, 4> diagnosis
	{
		2,
		3,
		8,
		4
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 9;
}

int ProcedureCreator::crownDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth[RootCanal],
		tooth[Fracture],
		tooth[Restoration],
		tooth[Implant]
	};

	std::array<int, 4> diagnosis
	{
		8,
		4,
		1, //should be extensive restoration
		5
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::implantDiagnosis(const Tooth& tooth)
{
	if (tooth[Missing]) return 5;

	return 0;
}

RestorationData ProcedureCreator::autoSurfaces(const Tooth& tooth)
{
	RestorationData result;

	auto& surfaces = result.surfaces;

	for (int i = 0; i < 6; i++)
	{
		surfaces[i] = tooth.hasCaries(i);
	}

	if (tooth[Root])
	{
		surfaces = { 1, 1, 1, 1, 1, 1 };

		if (!tooth[Post]) {
			result.post = true;
		}
	}

	if (tooth[RootCanal])
	{
		if (tooth.type() == Frontal)
		{
			surfaces[Surface::Lingual] = true;
		}
		else surfaces[Surface::Occlusal] = true;
	}

	if (tooth[Fracture])
	{
		if (tooth.type() == Frontal)
		{
			surfaces[Surface::Occlusal] = true;
		}
		else surfaces[Surface::Lingual] = true;
	}
	else if (tooth[Restoration] && !tooth[Caries])
	{
		for (int i = 0; i < 6; i++)
		{
			if (tooth.hasRestoration(i))
				surfaces[i] = true;
		}
	}

	return result;
}

ConstructionRange ProcedureCreator::getBridgeRange(const std::vector<const Tooth*> selectedTeeth, ProcedureCode code)
{

	if (code.type() == ProcedureType::Denture) {

		static const std::set<std::string> upperDenture{
			"97710-00",
			"97711-01",
			"97711-11",
			"97711-02",
			"97711-12",
			"97721-01",
			"97721-02",
			"97729-00"
		};

		if (upperDenture.count(code.code())) {
			return ConstructionRange{ 1, 14 };
		}

		static const std::set<std::string> lowerDenture{
			"97710-01",
			"97712-01",
			"97712-11",
			"97712-02",
			"97712-12",
			"97728-01",
			"97728-02",
			"97729-01"
		};

		if (lowerDenture.count(code.code())) {
			return ConstructionRange{ 17, 30 };
		}
	}

	if (!selectedTeeth.size()) return { 0, 1 };

	if (selectedTeeth.size() == 1)
	{
		return { selectedTeeth[0]->index(), selectedTeeth[0]->index() };
	}

	int begin = selectedTeeth[0]->index(); //if multiple teeth are selected, the range is calculated to be valid
	int end = selectedTeeth.back()->index(); //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15) { end = 15; }

	return { begin, end };
}


void ProcedureCreator::setView(IProcedureInput* view)
{
	this->view = view;

	if (m_selectedTeeth.size()) {

		auto t = m_selectedTeeth[0];

		diag_map[ProcedureType::Restoration] = restorationDiagnosis(*t);
		diag_map[ProcedureType::Extraction] = extractionDiagnosis(*t);
		diag_map[ProcedureType::Endodontic] = endodonticDiagnosis(*t);
		diag_map[ProcedureType::Crown] = crownDiagnosis(*t);
		diag_map[ProcedureType::Implant] = implantDiagnosis(*t);
	}

	diag_map[ProcedureType::Bridge] = 5;
	diag_map[ProcedureType::Splint] = 7;
	diag_map[ProcedureType::Denture] = 6;
	diag_map[ProcedureType::Depuratio] = 10;


	view->setCurrentPresenter(this);

}

void ProcedureCreator::diagnosisChanged(int idx)
{
	diag_map[m_code.type()] = idx;
}