#include "InsuranceStatus.h"

std::string InsuranceStatus::getYearsText() const
{
	if (status == Insured::NoData) return "За този пациент не са открити данни в НАП";

	std::string result =
		status == Insured::Yes ?
		"Пациентът е здравно осигурен."
		:
		"Пациентът не е здравно осигурен!";

	if (yearsMonths.size()) {
		result += "\n\nМесеци без здравно осигуряване: \t";
	}
	result.reserve(yearsMonths.size() * 20);

	for (auto [year, months] : yearsMonths) 
	{
		result += "\n";
		result += std::to_string(year) + ": ";

		for (auto month : months) {
			result += std::to_string(month) + ", ";
		}

		//removing the last comma:
		result.pop_back();
		result.pop_back();
	}

	return result;
}
