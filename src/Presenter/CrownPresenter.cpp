#include "CrownPresenter.h"

#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/KSMP.h"
#include "View/Interfaces/AbstractRangeEdit.h"

CrownPresenter::CrownPresenter(
	const std::vector<const Tooth*>& selectedTeeth,
	const ToothContainer& teeth
)
	:
	AbstractSubPresenter(ProcedureType::crown),
    teeth(teeth),
	selectedTeeth(selectedTeeth),
	m_bridgeSelected(!selectedTeeth.size()), //if no teeth are selected
	m_ksmpOther(KSMP::getByType(ProcedureType::bridge).at(1)->code)
{

	if (selectedTeeth.size() == 1)
	{
		m_diagnosis = getDiagnosis(*selectedTeeth.front());
	}


	auto range = getInitialRange();

	tooth_begin = range.tooth_begin;
	tooth_end = range.tooth_end;

}

void CrownPresenter::setAdditionalTemplateParameters()
{
	view->setLayout(ICommonFields::Crown);

	if (firstTimeSelected)
	{
		view->rangeWidget()->setBridgeRange(tooth_begin, tooth_end);
		firstTimeSelected = false;
	}

	if (selectedTeeth.empty()) //only bridge can be generated
	{
		view->setBridgeCheckState(ICommonFields::Checked);
		view->disableBridgeCheck(true);
		
	}


	if (m_bridgeSelected) {
		std::swap(m_ksmp, m_ksmpOther);
	}


	if (m_bridgeSelected) {
		std::swap(m_ksmp, m_ksmpOther);
	}

	bridgeChecked(m_bridgeSelected);

}

void CrownPresenter::bridgeChecked(bool checked)
{
	//condition in which fn is called from ui
	if (m_bridgeSelected != checked) {
		std::swap(m_ksmp, m_ksmpOther);
		
	} //else, they are already swapped in  setProcedureTemplate fn

	m_bridgeSelected = checked;
	view->setKSMPCode(m_ksmp);

	if (checked)
	{
		view->diagnosisEdit()->set_Text("Anodontia partialis");
		m_type = ProcedureType::bridge;
		view->rangeWidget()->setInputValidator(&range_validator);
		view->rangeWidget()->hide(false);
		return;
	}

	view->diagnosisEdit()->set_Text(m_diagnosis);
	view->procedureNameEdit()->set_Text(m_name);
	view->rangeWidget()->setInputValidator(nullptr);
	view->rangeWidget()->hide(true);
	m_type = ProcedureType::crown;

}

ConstructionRange CrownPresenter::getInitialRange()
{
	int begin;
	int end;

	if (!selectedTeeth.size())
	{
		begin = 0;
		end = 1;
		return { begin, end };
	}

	begin = 0;
	end = 0;

	if (selectedTeeth.size() == 1) //if only 1 tooth is selected, the bridge length is 2
	{
		begin = selectedTeeth[0]->index;

		if (begin != 15 && begin != 31)
		{
			end = begin + 1;
		}
		else
		{
			end = begin - 1;
			std::swap(end, begin);
		}
		return { begin, end };
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return { begin, end };
}

std::vector<Procedure> CrownPresenter::getProcedures()
{
	std::vector<Procedure> procedures;

	if(m_bridgeSelected)
	{
		auto procedure = AbstractSubPresenter::getProcedureCommonFields();
		procedure.type = ProcedureType::bridge;

		auto [begin, end] = view->rangeWidget()->getRange();

		procedure.result = ConstructionRange{
			begin, end
		};

		return { procedure };
	}


	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth) {
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().tooth = tooth->index;
		procedures.back().temp = tooth->temporary.exists();
	}

	return procedures;
}


bool CrownPresenter::additionalValidation()
{

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
	//bridgeLogic.setRange(begin, end);

	view->diagnosisEdit()->set_Text(getBridgeDiagnosis());
	//common_view->procedureNameEdit()->set_Text(m_name + bridgeLogic.rangeString());
//	common_view->priceEdit()->set_Value(bridgeLogic.price());
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
		"Фрактура",
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

std::string CrownPresenter::getBridgeDiagnosis()
{
	
	if (!view->rangeWidget()->isValid()) return bridgeDiagnosis;

	auto[begin, end] = view->rangeWidget()->getRange();

	for (int i = begin; i <= end; i++)
	{
		auto& tooth = teeth.at(i);

		if (tooth.extraction) return "Andontia partialis";
	}

	return "Стабилизация с блок корони";
}

