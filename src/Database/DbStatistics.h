#pragma once
#include <vector>
#include <string>

struct DentalStatistic;

namespace DbStat
{

	int count(const DentalStatistic& stat, int year);
	
	const std::vector<std::string> getProcedureNames(int procedureType);
	const std::vector<std::string> getDiagnosis(int procedureType);
	
}