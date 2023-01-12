#pragma once
#include <string>
#include <vector>
#include "Model/Dental/DentalStatistics.h"

class DentalStatistic;

class IStatisticDialogView
{
public:
	virtual void setNameFilterList(const std::vector<std::string> namesFilter) = 0;
	virtual void setDiagnosisFilterList(const std::vector<std::string> diagnosis) = 0;
	virtual void setStatistic(const DentalStatistic& s) = 0;
	virtual DentalStatistic getStatistic() = 0;
	virtual void closeDialog() = 0;
	virtual void hideToothFilter(bool hidden) = 0;
};