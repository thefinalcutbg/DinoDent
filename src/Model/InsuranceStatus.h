#pragma once
#include <vector>
#include <map>

typedef std::map<int, std::vector<int>> MissingPeriods;
enum class Insured {NoData, Yes, No};

struct InsuranceStatus {

	Insured status{ Insured::NoData };
	MissingPeriods yearsMonths;
};
