#pragma once
#include <vector>
#include <map>
#include <string>
typedef std::map<int, std::vector<int>> MissingPeriods;
enum class Insured {NoData, Yes, No};

struct InsuranceStatus {

	Insured status{ Insured::NoData };
	MissingPeriods yearsMonths;

	std::string getYearsText();

};
