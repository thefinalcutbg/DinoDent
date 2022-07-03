#include "InsuranceStatus.h"

std::string InsuranceStatus::getYearsText() const
{
	if (status == Insured::NoData) return u8"За този пациент не са открити данни в НАП";

	std::string result;
	
	if (yearsMonths.size()) {
		result += u8"Месеци без здравно осигуряване : \t\n\n";
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
		u8"Пациентът е здравно осигурен!"
		:
		u8"Пациентът не е здравно осигурен!";


	return result;
}
