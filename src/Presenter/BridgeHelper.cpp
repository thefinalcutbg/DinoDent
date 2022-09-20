#include "BridgeHelper.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/ToothUtils.h"


BridgeHelper::BridgeHelper(const ToothContainer& t, const std::vector<Tooth*>& selT)
	:
	teeth(t),
	selectedTeeth(selT)
{
	getInitialBridgeRange();
	setRange(begin, end);
}

void BridgeHelper::getInitialBridgeRange()
{

	if (!selectedTeeth.size())
	{
		begin = 0;
		end = 1;
		return;
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
		return;
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return;

}

void BridgeHelper::setRange(int begin, int end)
{
	if (begin > end) std::swap(begin, end);

	this->begin = begin;
	this->end = end;

	setPrice(crown_price);
	refreshDiagnosis();
	refreshRangeString();

}


void BridgeHelper::refreshDiagnosis()
{
	std::vector<const Tooth*> teethRange;

	if (!validator.validateInput(begin, end)) {
		diagnosis.clear();
	}

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
		diagnosis = u8"Всички зъби са извадени, wtf?"; //all selected teeth are extracted, wtf user?
		return;
	}

	if (!missingTeethIdx.size())
	{
		diagnosis = u8"Стабилизация с блок корони";
		return;
	}

	diagnosis.clear();
	diagnosis.append("Adontia partialis ");

	for (int i = 0; i < missingTeethIdx.size(); i++)
	{

		diagnosis.append(std::to_string(missingTeethIdx.at(i)));

		if (i != missingTeethIdx.size() - 1)
		{
			diagnosis.append(", ");
		}
	}

}

void BridgeHelper::refreshRangeString()
{
	range =
		" (конструкция " +
		std::to_string(ToothUtils::getToothNumber(begin, teeth[begin].temporary.exists())) +
		" - " +
		std::to_string(ToothUtils::getToothNumber(end, teeth[end].temporary.exists())) + ")";

}


void BridgeHelper::setPrice(double crownPrice)
{
	crown_price = crownPrice;

	bridge_price = validator.validateInput(begin, end) ?

	crown_price * (end - begin + 1)
	:
	0;
		
		
}