#include "InsuranceStatus.h"

std::string InsuranceStatus::getYearsText()
{
	if (status != Insured::No) return {};

	std::string result{ u8"Месеци без здравно осигуряване : \t\n\n" };

	result.reserve(yearsMonths.size() * 20);

	for (auto [year, months] : yearsMonths) 
	{
		result.append(std::to_string(year) + ": ");

		for (auto month : months) {
			result.append(std::to_string(month) + ", ");
		}

		//turning the last comma into newline
		result.pop_back();
		result.back() = '\n';

	}

	return result;
}
