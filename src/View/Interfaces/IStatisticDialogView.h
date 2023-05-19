#pragma once
#include <string>
#include <vector>
#include "Model/Dental/DentalStatistics.h"
#include "Model/Dental/ProcedureCode.h"

class DentalStatistic;

class IStatisticDialogView
{
public:
	virtual void setProcedureFilter(const std::vector<ProcedureCode> namesFilter) = 0;
	virtual void setStatistic(const DentalStatistic& s) = 0;
	virtual DentalStatistic getStatistic() = 0;
	virtual void closeDialog() = 0;
	virtual void hideToothFilter(bool hidden) = 0;
	virtual void setResult(int count) = 0;
};