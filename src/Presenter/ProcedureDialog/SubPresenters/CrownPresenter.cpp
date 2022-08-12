#include "CrownPresenter.h"

#include "Model/Tooth/ToothContainer.h"
#include "Model/KSMP.h"
CrownPresenter::CrownPresenter(
	const std::vector<Tooth*>& selectedTeeth,
	const ToothContainer& teeth
)
	:
	AbstractSubPresenter(ProcedureType::crown),
    teeth(teeth),
	selectedTeeth(selectedTeeth),
	view(nullptr),
	m_bridgeSelected(!selectedTeeth.size()), //if no teeth are selected
	bridgeLogic(teeth, selectedTeeth),
	m_ksmpOther(KSMP::getByType(ProcedureType::bridge).at(1)->code)
{

	if (selectedTeeth.size() == 1)
	{
		m_diagnosis = getDiagnosis(*selectedTeeth.front());
	}

}

void CrownPresenter::setView(ICrownView* view)
{
	this->view = view;

	view->rangeWidget()->setBridgeRange(bridgeLogic.Begin(), bridgeLogic.End());

	view->rangeWidget()->setInputValidator(&range_validator);

	if (selectedTeeth.empty()) //only bridge can be generated
	{
		view->lockBridgeCheckbox(true);
	}
}


void CrownPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	if (m_bridgeSelected) {
		std::swap(m_ksmp, m_ksmpOther);
	}
	
	AbstractSubPresenter::setProcedureTemplate(m);

	if (m_bridgeSelected) {
		std::swap(m_ksmp, m_ksmpOther);
	}

	bridgeLogic.setPrice(m.price);

	view->setMaterial(m.material);

	selectAsBridge(m_bridgeSelected);

}

void CrownPresenter::selectAsBridge(bool checked)
{
	//condition in which fn is called from ui
	if (m_bridgeSelected != checked) {
		std::swap(m_ksmp, m_ksmpOther);
		
	} //else, they are already swapped in  setProcedureTemplate fn

	m_bridgeSelected = checked;
	common_view->setKSMPCode(m_ksmp);

	if (checked)
	{
		common_view->diagnosisEdit()->set_Text(bridgeLogic.Diagnosis());
		common_view->procedureNameEdit()->set_Text(m_name + bridgeLogic.rangeString());
		common_view->priceEdit()->set_Value(bridgeLogic.price());
		m_type = ProcedureType::bridge;
		return;
	}

	common_view->diagnosisEdit()->set_Text(m_diagnosis);
	common_view->procedureNameEdit()->set_Text(m_name);
	common_view->priceEdit()->set_Value(m_price);
	m_type = ProcedureType::crown;

}

std::vector<Procedure> CrownPresenter::getProcedures()
{
	std::vector<Procedure> procedures;

	if (!m_bridgeSelected)
	{
		procedures.reserve(selectedTeeth.size());

		for (auto& tooth : selectedTeeth) {
			procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
			procedures.back().result = view->getData();
			procedures.back().tooth = tooth->index;
			procedures.back().temp = tooth->temporary.exists();
		}

		return procedures;
	}

	
	auto procedure = AbstractSubPresenter::getProcedureCommonFields();
	procedure.type = ProcedureType::bridge;

	auto data = view->getData();

	procedure.result = ProcedureBridgeData{
		bridgeLogic.Begin(), bridgeLogic.End(), data
	};

	return {procedure};
}

bool CrownPresenter::isValid()
{
	if (!AbstractSubPresenter::isValid()) return false;

	if (m_type == ProcedureType::crown &&
		!selectedTeeth.size()) return false;

	if (m_bridgeSelected)
	{
		auto rW = view->rangeWidget();
		
		rW->validateInput();
		if (!rW->isValid())
		{
			rW->setFocus();
			return false;
		}
	}

	return true;
}

void CrownPresenter::rangeChanged(int begin, int end)
{
	bridgeLogic.setRange(begin, end);

	common_view->diagnosisEdit()->set_Text(bridgeLogic.Diagnosis());
	common_view->procedureNameEdit()->set_Text(m_name + bridgeLogic.rangeString());
	common_view->priceEdit()->set_Value(bridgeLogic.price());
}

std::string CrownPresenter::getDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.endo.exists(),
		tooth.fracture.exists(),
		tooth.obturation.exists(),
		tooth.implant.exists()
	};

	std::array<std::string, 4> diagnosis
	{
		"Status post devitalisationem",
		tooth.fracture.data.getDiagnosisName(),
		"Екстензивна ресторация",
		"Протезиране върху имплант"
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}


