#pragma once
#include "Model/Financial/MonthNotifRow.h"
#include <vector>

class IMonthNotifView
{
public:
	virtual void setMonthNotifRows(const std::vector<MonthNotifRow> rows) = 0;
	virtual void closeParentDialog() = 0;
};