#include "PackageCounter.h"

PackageCounter::PackageCounter(const std::vector<ProcedurePackage>& packages)
	:
    packages(packages), procedure_count(packages.size(), 0)
{}

void PackageCounter::insertCode(int code)
{
    for (size_t i = 0; i < packages.size(); i++)
		if (packages[i].codes.count(code))
			procedure_count[i]++;
}

bool PackageCounter::validate(bool adult)
{

    for (size_t i = 0; i < packages.size(); i++)
	{
		const auto& package = packages[i];

		int limit = adult ? package.limit_adult : package.limit_minor;

		if (procedure_count[i] > limit)
		{
			return false;
		}
	}
	
	return true;
}

