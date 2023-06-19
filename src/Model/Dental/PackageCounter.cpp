#include "PackageCounter.h"

PackageCounter::PackageCounter(const std::vector<ProcedurePackage>& packages)
	:
	packages(packages)
{
	for (auto& p : packages)
	{
		procedure_count.push_back(0);
	}
}

void PackageCounter::insertCode(int code)
{
	for (int i = 0; i < packages.size(); i++)
		if (packages[i].codes.count(code))
			procedure_count[i]++;
}

bool PackageCounter::validate(bool adult)
{

	for (int i = 0; i < packages.size(); i++)
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

