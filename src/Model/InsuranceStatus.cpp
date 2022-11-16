#include "InsuranceStatus.h"

std::string InsuranceStatus::getYearsText() const
{
	if (status == Insured::NoData) return "За този пациент не са открити данни в НАП";

	std::string result;
	
	if (yearsMonths.size()) {
		result += "Месеци без здравно осигуряване : \t\n\n";
	}
	result.reserve(yearsMonths.size() * 20);

	for (auto [year, months] : yearsMonths) 
	{
		result += std::to_string(year) + ": ";

		for (auto month : months) {
			result += std::to_string(month) + ", ";
		}

		//turning the last comma into newline
		result.pop_back();
		result.back() = '\n';

	}

	result +=
		status == Insured::Yes ?
		"Пациентът е здравно осигурен!"
		:
		"Пациентът не е здравно осигурен!";


	return result;
}
