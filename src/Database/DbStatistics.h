#pragma once
#include <vector>
#include <string>

#include "Model/Date.h"

class DentalStatistic;

namespace DbStat
{

	int procedureCount(const DentalStatistic& stat, const Date& from, const Date& to);

	const std::vector<std::string> getProcedureNames(int procedureType);
	const std::vector<std::string> getDiagnosis(int procedureType);
	
}