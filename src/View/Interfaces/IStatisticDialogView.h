#pragma once

#include <vector>
#include "Model/Dental/DentalStatistics.h"
#include "Model/Dental/ProcedureCode.h"

class IStatisticDialogView
{
public:
	virtual void setProcedureFilter(const std::vector<ProcedureCode> namesFilter) = 0;
	virtual DentalStatistic getStatistic() = 0;
	virtual void closeDialog() = 0;
	virtual void hideToothFilter(bool hidden) = 0;
	virtual void setResult(int count) = 0;
};
