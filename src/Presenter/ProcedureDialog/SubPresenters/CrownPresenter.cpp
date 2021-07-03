#include "CrownPresenter.h"
#include <QDebug>
CrownPresenter::CrownPresenter(
    const std::vector<Tooth*>& selectedTeeth, 
    const std::array<Tooth, 32>& teeth
) 
    :
	TeethMPresenter(selectedTeeth),
    teeth(&teeth),
	view(nullptr),
	bridge(false),
	bridgePrice(0)
{

	if (selectedTeeth.size() == 1)
	{
		diagnosis = autoDiagnosis(*this->selectedTeeth->at(0));
	}
	
}

void CrownPresenter::setManipulationTemplate(const ProcedureTemplate& m)
{

	common_view->set_hidden(noTeethSelected);
	view->set_hidden(noTeethSelected);
	if (noTeethSelected) return;

	GeneralMPresenter::setManipulationTemplate(m);

	view->materialEdit()->set_Text(m.material);
	
	auto [begin, end] = view->rangeWidget()->getRange();
	int length = end - begin + 1;
	bridgePrice = m_template.price * length;

	if (!range_validator.validateInput(begin, end))
		bridgePrice = 0;

	if (bridge)
		rangeWidgetChecked(true); //magic!

}

std::vector<Procedure> CrownPresenter::getManipulations()
{
	if(!bridge) return TeethMPresenter::getManipulations();
	
	auto manipulation = GeneralMPresenter::getManipulations();
	manipulation[0].type = ProcedureType::bridge;
	return manipulation;
	
					
}

bool CrownPresenter::isValid()
{
	if (!TeethMPresenter::isValid()) return false;

	if (bridge)
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


void CrownPresenter::setView(ICrownView* view)
{
	this->view = view;
	auto [begin, end] = getInitialBridgeRange(*selectedTeeth);

	view->rangeWidget()->setRange(begin, end);
	bridgeDiagnosis = getBridgeDiagnosis(begin, end, *teeth);
	bridgeRangeName = getBridgeRangeName(begin, end, *teeth);


	view->rangeWidget()->setInputValidator(&range_validator);

}

Result CrownPresenter::getResult()
{


	if (bridge)
	{
		auto [begin, end] = view->rangeWidget()->getRange();
		auto crown = view->getData();
		return BridgeData { crown, begin, end };
	}

	return view->getData();
}

void CrownPresenter::rangeChanged(int begin, int end)
{
	bridgeDiagnosis = getBridgeDiagnosis(begin, end, *teeth);
	bridgeRangeName = getBridgeRangeName(begin, end, *teeth);

	int length = end - begin + 1;
	bridgePrice = m_template.price * length;

	common_view->diagnosisEdit()->set_Text(bridgeDiagnosis);
	common_view->manipulationEdit()->set_Text(m_template.name + bridgeRangeName);
	common_view->priceEdit()->set_Value(bridgePrice);
}





void CrownPresenter::rangeWidgetChecked(bool checked)
{
	if(checked)
	{
		bridge = true;
		common_view->diagnosisEdit()->set_Text(bridgeDiagnosis);
		common_view->manipulationEdit()->set_Text(m_template.name + bridgeRangeName);
		common_view->priceEdit()->set_Value(bridgePrice);

		return;
	}

	bridge = false;
	common_view->diagnosisEdit()->set_Text(diagnosis);
	common_view->manipulationEdit()->set_Text(m_template.name);
	common_view->priceEdit()->set_Value(m_template.price);
}

std::string CrownPresenter::autoDiagnosis(const Tooth& tooth)
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
		tooth.fracture.getDiagnosis(),
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

std::tuple<int, int> CrownPresenter::getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth)
{
	if (!selectedTeeth.size())
	{
		return std::tuple<int, int>(0, 1);
	}

	int begin = 0;
	int end = 0;

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
		return std::tuple<int, int>{begin, end};
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return std::tuple<int, int>(begin, end);
}

std::string CrownPresenter::getBridgeDiagnosis(int begin, int end, const std::array<Tooth, 32>& teeth)
{
	std::vector<const Tooth*> teethRange;

	for (int i = begin; i < end + 1; i++)
	{
		teethRange.push_back(&teeth.at(i));
	}

	std::vector<int> missingTeethIdx;

	for (const Tooth* tooth : teethRange)
	{
		if (tooth->extraction.exists())
		{
			missingTeethIdx.emplace_back(ToothUtils::getToothNumber(tooth->index, false));
		}
	}


	if (missingTeethIdx.size() == teethRange.size())
	{
		return "Всички зъби са извадени, wtf?"; //all selected teeth are extracted, wtf user?
	}

	if (!missingTeethIdx.size())
	{
		return "Стабилизация с блок корони";
	}

	std::string bridgeDiagnosis;
	bridgeDiagnosis.reserve(5 * missingTeethIdx.size() + 19);
	bridgeDiagnosis.append("Adontia partialis ");

	for (int i = 0; i < missingTeethIdx.size(); i++)
	{

		bridgeDiagnosis.append(std::to_string(missingTeethIdx.at(i)));

		if (i != missingTeethIdx.size() - 1)
		{
			bridgeDiagnosis.append(", ");
		}
	}

	return bridgeDiagnosis;
}

std::string CrownPresenter::getBridgeRangeName(int begin, int end, const std::array<Tooth, 32>& teeth)
{
	return std::string
	{
		" (конструкция " +
		std::to_string(ToothUtils::getToothNumber(begin, teeth[begin].temporary.exists())) +
		" - " +
		std::to_string(ToothUtils::getToothNumber(end, teeth[end].temporary.exists())) + ")"
	};
}