#include "ProcedureCreator.h"
#include <set>
#include "Model/User.h"

using namespace Dental;

ProcedureCreator::ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth)
	: m_selectedTeeth(selectedTeeth)
{


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

std::array<bool, 6> ProcedureCreator::autoSurfaces(const Tooth& tooth)
{
	std::array<bool, 6> surfaces{ 0,0,0,0,0,0 };

	for (int i = 0; i < 6; i++)
	{
		surfaces[i] = tooth.hasCaries(i);
	}

	if (tooth[Root])
	{
		surfaces = { 1, 1, 1, 1, 1, 1 };
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

	return surfaces;
}

ConstructionRange ProcedureCreator::getBridgeRange(const std::vector<const Tooth*> selectedTeeth)
{
	if (!selectedTeeth.size()) return { 0, 0 };
	
	if (selectedTeeth.size() == 1)
	{
		return { selectedTeeth[0]->index(), selectedTeeth[0]->index() };
	}

	int begin = selectedTeeth[0]->index(); //if multiple teeth are selected, the range is calculated to be valid
	int end = selectedTeeth.back()->index(); //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15) { end = 15; }

	return { begin, end };
}

std::string ProcedureCreator::bridgeOrFiberDiagnosis(const std::vector<const Tooth*> selectedTeeth, const ConstructionRange& range)
{
	for (int i = range.tooth_begin; i <= range.tooth_end; i++)
	{
		auto& tooth = *selectedTeeth.at(i);

		if (tooth[Missing]) return "Andontia partialis";
	}

	return "Стабилизация с блок корони";
}

void ProcedureCreator::setView(IProcedureInput* view)
{
	this->view = view;

	if (m_selectedTeeth.size()) {

		auto t = m_selectedTeeth[0];

		view->surfaceSelector()->setData(RestorationData{ autoSurfaces(*t), false });
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

	auto [begin, end] = getBridgeRange(m_selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);

	view->setCurrentPresenter(this);



	//diag_map[ProcedureType::Bridge] = bridgeOrFiberDiagnosis(m_selectedTeeth, ConstructionRange{ begin, end });
	//diag_map[ProcedureType::Splint] = bridgeOrFiberDiagnosis(m_selectedTeeth, ConstructionRange{ begin, end });

}

void ProcedureCreator::diagnosisChanged(int idx)
{
	diag_map[m_code.type()] = idx;
}

void ProcedureCreator::setProcedureCode(const ProcedureCode& m, bool nhif)
{
	view->setCurrentPresenter(this);

	m_code = m;

	view->setNhifLayout(nhif);

	range_validator.allowSingleRange = m.type() == ProcedureType::Denture;

	//GENERAL
	if (m.isGeneral()) {

		view->surfaceSelector()->setInputValidator(nullptr);
		view->rangeWidget()->setInputValidator(nullptr);

		m.isAnesthesia() ? 
			view->setLayout(IProcedureInput::Anesthesia)
			:
			view->setLayout(IProcedureInput::General);

	}

	//TOOTH SPECIFIC
	if (m.isToothSpecific()) {

		if (m_selectedTeeth.empty()) {
			view->setErrorMsg("Изберете поне един зъб!");
			
		}
		else {
			
			view->rangeWidget()->setInputValidator(nullptr);

			if (m.isRestoration()) {

				view->setLayout(IProcedureInput::Restoration);
				view->surfaceSelector()->setInputValidator(&surface_validator);
			}
			else {
				view->setLayout(IProcedureInput::ToothSpecific);
			}
		}
	}

	//RANGE SPECIFIC
	if (m.isRangeSpecific()) {

		view->setLayout(IProcedureInput::Range);
		view->surfaceSelector()->setInputValidator(nullptr);

		view->rangeWidget()->setInputValidator(

			m_code.requiresRangeValidation() ?
				&range_validator
				:
				nullptr
		);

		auto [begin, end] = getBridgeRange(m_selectedTeeth);
		view->rangeWidget()->setBridgeRange(begin, end);
	} 


	auto diagIdx = diag_map[m.type()];

	view->diagnosisCombo()->setIndex(diag_map[m.type()]);

	/*
	//this logic should be...elsewhere
	//total denture nhif manifacturing
	if (!diagIdx && (m_code.oldCode() == 834 || m_code.oldCode() == 835))
	{
		view->diagnosisCombo()->setIndex(6);
		diagIdx = 6;
	}

	view->diagnosisEdit()->setInputValidator(diagIdx ? nullptr : &notEmpty_validator);

	const std::set<int> exams = { 101, 102, 103 };

	if (!diagIdx && exams.count(m.oldCode())) {
		view->diagnosisEdit()->set_Text("Преглед");
	}
	else
	{
		view->diagnosisEdit()->set_Text("");
	}
	*/
	view->diagnosisEdit()->validateInput();


}

bool ProcedureCreator::isValid()
{

	std::vector<AbstractUIElement*> validatable{
		view->dateEdit(),
		view->surfaceSelector(),
		view->rangeWidget(),
		view->diagnosisEdit()
	};


	for (AbstractUIElement* e : validatable)
	{
		e->validateInput();
		if (!e->isValid())
		{
            e->set_focus();
			return false;
		}
	}

	return true;
}

std::vector<Procedure> ProcedureCreator::getProcedures()
{
	Procedure procedure;

	std::vector<Procedure> result;

	procedure.code = m_code;

	procedure.financingSource = 
		procedure.code.oldCode() ? FinancingSource::NHIF 
		: 
		view->getFinancingSource();

	procedure.LPK = User::doctor().LPK;
	procedure.date = view->dateEdit()->getDate();
	procedure.diagnosis = view->diagnosisCombo()->getIndex();
	procedure.diagnosis.description = view->diagnosisEdit()->getText();
	procedure.notes = view->getNotes();
	procedure.tooth_idx.supernumeral = view->onHyperdontic();
	procedure.result = std::monostate{};

	if (procedure.code.isGeneral()) {

		if (procedure.code.isAnesthesia()) {
			procedure.result = AnesthesiaMinutes{ view->minutes() };
		}

		result.push_back(procedure);
	}
	else if (procedure.code.isToothSpecific()) {

		if (procedure.code.isRestoration()) {

			procedure.result = view->surfaceSelector()->getData();
		}

		for (auto t : m_selectedTeeth)
		{
			result.push_back(procedure);
			result.back().tooth_idx.index = t->index();
		}

	}
	else if (procedure.code.isRangeSpecific()) {

		auto [begin, end] = view->rangeWidget()->getRange();
		procedure.result = ConstructionRange{ begin, end };
		result.push_back(procedure);

	}

	return result;
}



