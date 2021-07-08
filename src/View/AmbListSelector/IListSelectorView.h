#pragma once

#include <vector>

#include "Model/AmbListRow.h"

class IListSelectorView
{
public:
	virtual void addYearsToCombo(const std::vector<int>& years) = 0;
	virtual void setUI(int month, int year) = 0;
	virtual void setRows(const std::vector<AmbListRow>& rows) = 0;
	virtual void focus() = 0;
};