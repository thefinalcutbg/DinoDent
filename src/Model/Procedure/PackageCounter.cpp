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
#include <qDebug>
bool PackageCounter::validate(bool adult, bool pregnant)
{

	//if (mental && !adult) return true;

	for (int i = 0; i < packages.size(); i++)
	{
		const auto& package = packages[i];

		int limit = adult ? package.limit_adult : package.limit_minor;
		if(pregnant) limit = std::max(package.limit_pregnant, limit);

		if (procedure_count[i] > limit)
		{
			qDebug() << "Надхвърлен лимит в пакет: " << i << "\nЛимит: " << limit << "\nБрой манипулации: " << procedure_count[i];
			return false;
		}
	}
	
	return true;
}

