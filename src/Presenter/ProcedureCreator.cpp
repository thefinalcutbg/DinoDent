#include "ProcedureCreator.h"
#include <set>
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"

ProcedureCreator::ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth)
	: m_selectedTeeth(selectedTeeth)
{
	auto tooth = m_selectedTeeth.size() ? m_selectedTeeth.at(0) : nullptr;

	for (auto i = 0; i < static_cast<int>(ProcedureType::MaxCount); i++) {

		diag_map[ProcedureType(i)] = getDiagnosis(tooth, ProcedureType(i));
	}

}

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
		m_code = ProcedureCode();
		return;
	}

	//setting common data
	IProcedureInput::CommonData commonData;

	if (User::settings().autoDiagnosis){
		auto & defaultICDCode = code.defaultICD10();

		if (defaultICDCode.size()) {
			commonData.diagnosis.icd = defaultICDCode;
		}
		else {
			commonData.diagnosis = diag_map[code.type()];
		}
	}

	commonData.financingSource = nhif ? FinancingSource::NHIF : s_preferred_financing;
	commonData.value = value;

	//setting parameter data
	if (code.type() == ProcedureType::Anesthesia) {
		view->setParameterData(AnesthesiaMinutes(180));
	}
	else if (code.getScope() == ProcedureScope::AllOrNone) {
		view->setParameterData();
	}
	else if (code.type() == ProcedureType::Restoration) {
		view->setParameterData(false, autoSurfaces(*m_selectedTeeth[0]));
	}
	else if (code.type() == ProcedureType::RemoveRestoration) {

		auto rData = autoSurfaces(*m_selectedTeeth[0]);

		if (m_selectedTeeth.at(0)->hasStatus(Dental::Post)) {
			rData.post = true;
		}

		view->setParameterData(false, autoSurfaces(*m_selectedTeeth[0]));

	}
	else if (code.getScope() == ProcedureScope::SingleTooth) {
		view->setParameterData(false);
	}
	else if (code.getScope() == ProcedureScope::Range) {

		bool allowSingleRange =
			code.type() == ProcedureType::RemoveCrownOrBridge
			|| code.type() == ProcedureType::Denture;

		view->setParameterData(autoRange(m_selectedTeeth, m_code), allowSingleRange);
	}
	else if (code.type() == ProcedureType::Crown) {
		view->setParameterData(false, autoRange(m_selectedTeeth, m_code), true);
	}
	else if (code.getScope() == ProcedureScope::Ambi) {

		auto range = autoRange(m_selectedTeeth, m_code);

		//single crown
		int preferredIdx = 0;
		commonData.diagnosis = diag_map[ProcedureType::Crown];

		//range
		if (range.getTeethCount() > 1) {
			commonData.diagnosis = diag_map[ProcedureType::Bridge];
			preferredIdx = 1;
		}
		//veneer
		else if (m_selectedTeeth[0]->type() == Dental::Type::Frontal
			&& !m_selectedTeeth[0]->hasStatus(Dental::Status::RootCanal)) 
		{
			commonData.diagnosis = diag_map[ProcedureType::Restoration];
			preferredIdx = 2;
		}

		view->setParameterData(false, range, RestorationData{ {1,0,0,1,0},0 }, preferredIdx);
	}


	view->setCommonData(commonData, code.nhifCode());
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

	auto data = view->getResult();

	if (data.financingSource != FinancingSource::NHIF) {
		s_preferred_financing = data.financingSource;
	}

	if (m_code.nhifCode()
		&& data.financingSource != FinancingSource::NHIF
		&& !ModalDialogBuilder::askDialog(
			"Посоченият източник на финансиране е различен от НЗОК "
			"и процедурата няма да бъде включена в месечния отчет."
			" Желаете ли да продължите?"
		)
	)
	{
		return {};
	}

	procedure.financingSource = data.financingSource;
	procedure.LPK = User::doctor().LPK;
	procedure.diagnosis = data.diagnosis;
	procedure.notes = data.notes;

	bool supernumeral = false;

	//range
	if (std::holds_alternative<ConstructionRange>(data.parameters)) {
		procedure.affectedTeeth = std::get<ConstructionRange>(data.parameters);
		return { procedure };
	}
	//anesthesia
	else if (std::holds_alternative<int>(data.parameters)) {
		procedure.param = AnesthesiaMinutes(std::get<int>(data.parameters));
		return { procedure };
	}
	else if (m_code.getScope() == ProcedureScope::AllOrNone) {
		return { procedure };
	}
	//restoration
	else if (std::holds_alternative<std::pair<bool, RestorationData>>(data.parameters)) {
		
		auto& pair = std::get<std::pair<bool, RestorationData >>(data.parameters);
		supernumeral = pair.first;
		procedure.param = pair.second;
	}
	//tooth specific
	else if (std::holds_alternative<bool>(data.parameters)) {
		supernumeral = std::get<bool>(data.parameters);
	}


	//return separate procedures

	for (auto t : m_selectedTeeth)
	{
		auto index = t->toothIndex();
		index.supernumeral = supernumeral;
		procedure.affectedTeeth = index;

		//checks if the procedure is restoration and post is enabled
		bool hasPostProcedure =
			!m_code.isLegacy()
			&& std::holds_alternative<RestorationData>(procedure.param)
			&& std::get<RestorationData>(procedure.param).post;

		//adding post as separate procedure
		if (hasPostProcedure) {

			Procedure postProcedure = procedure;

			//changing the financing source
			if (postProcedure.financingSource == FinancingSource::NHIF) {
				postProcedure.financingSource = FinancingSource::None;
			}

			if (m_code.type() == ProcedureType::RemoveRestoration) {
				postProcedure.code = ProcedureCode("97452-00");
			}
			else {
				postProcedure.code = ProcedureCode("97594-01");
			}
			postProcedure.param = std::monostate{};
			
			result.push_back(postProcedure);
		}

		result.push_back(procedure);

		
		if (hasPostProcedure) {
			//removing the post flag
			std::get<RestorationData>(result.back().param).post = false;

			//first we remove the restoration and then we remove the post
			if (m_code.type() == ProcedureType::RemoveRestoration) {
				std::swap(result[result.size() - 1], result[result.size() - 2]);
			}
		}
	}

	return result;
}

Diagnosis ProcedureCreator::getDiagnosis(const Tooth* tooth, ProcedureType type)
{
	using namespace Dental;
	std::array<std::string, Status::StatusCount> icdSimple{};

	icdSimple[Caries] = "K02.1";
	icdSimple[Pulpitis] = "K04.0";
	icdSimple[ApicalLesion] = "K04.5";
	icdSimple[Periodontitis] = "K05.4";
	icdSimple[Fracture] = "S02.5";
	icdSimple[Root] = "S02.5";
	icdSimple[Mobility] = "S03.2";
	icdSimple[Temporary] = "K08.0";
	icdSimple[Impacted] = "K01";
	icdSimple[Denture] = "Z97.2";
	icdSimple[Missing] = "K08.1";
	icdSimple[HasSupernumeral] = "K00.1";
	icdSimple[Calculus] = "K03.6";
	icdSimple[RootCanal] = "K04.9";

	std::vector<Status> statusSearch;

	std::string icd;
	std::string description;

	switch (type)
	{
	case ProcedureType::General:
	case ProcedureType::Post:
	case ProcedureType::ToothNonSpecific:
	case ProcedureType::Anesthesia:
		break;

	case ProcedureType::FullExam: icd = "Z01.2";
		break;

	case ProcedureType::Depuratio:
	case ProcedureType::DepuratioQuadrant:
	case ProcedureType::DepuratioTooth: 
		icd = icdSimple[Calculus];
		statusSearch = {Periodontitis, Calculus, Mobility };
		break;

	case ProcedureType::DenturePair:
	case ProcedureType::Denture:
	case ProcedureType::Implant:
	case ProcedureType::Bridge:	icd = icdSimple[Missing];
		break;

	case ProcedureType::Restoration:
		icd = "K03.7";
		statusSearch = { Fracture, Caries, Pulpitis, ApicalLesion, Root };
		break;
	case ProcedureType::RemoveRestoration:
		statusSearch = { Pulpitis, ApicalLesion, Fracture, Caries, Periodontitis };
		break;
	case ProcedureType::Extraction:

		icdSimple[Implant] = "T85.7";

		if (tooth && tooth->hasStatus(Root) && tooth->hasStatus(Impacted)) {
			icdSimple[Root] = "K08.3";
			icdSimple[Impacted] = "K08.3";
		}

		icd = "K07.3"; //assume ortho reason
		statusSearch = { Implant, Impacted, HasSupernumeral, ApicalLesion, Root, Periodontitis, Mobility, Fracture, Pulpitis, Temporary, Caries };
		break;

	case ProcedureType::Endodontic:
		statusSearch = { Pulpitis, ApicalLesion, Root, Fracture, Periodontitis, RootCanal };
		break;

	case ProcedureType::RemovePost:
		statusSearch = { ApicalLesion, Root, Fracture, RootCanal };
		break;

	case ProcedureType::PostCore:
		statusSearch = { Root, Fracture };
		break;

	case ProcedureType::PostCrown:
		statusSearch = { ApicalLesion, Root, Fracture, RootCanal };
		break;
		
	case ProcedureType::Crown:
		icd = "K03.7";
		icdSimple[Implant] = "Z96.5";
		statusSearch = {Implant, Fracture, RootCanal};
		break;
	case ProcedureType::CrownOrBridgeOrVeneer:

		break;
	case ProcedureType::RemoveCrownOrBridge:
		statusSearch = { Pulpitis, ApicalLesion, Root, Fracture, Periodontitis, RootCanal };
		break;

	case ProcedureType::MultipleExtraction:
		break;
	}

	for (auto s : statusSearch) {

		if (tooth == nullptr) break;

		if (tooth->hasStatus(s)) {
			
			icd = icdSimple[s];
/*
			if (s == RootCanal) {
				description = "K04.99 - Девитализиран зъб";
			}
*/
			break;
		}
	}

	Diagnosis d;
	d.icd = icd;
	d.additional_descr = description;

	return d;
}

RestorationData ProcedureCreator::autoSurfaces(const Tooth& tooth)
{
	using namespace Dental;

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

ConstructionRange ProcedureCreator::autoRange(const std::vector<const Tooth*> selectedTeeth, ProcedureCode code)
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

	view->setCurrentPresenter(this);
}

void ProcedureCreator::diagnosisChanged(int idx)
{
	//diag_map[m_code.type()] = idx;
}