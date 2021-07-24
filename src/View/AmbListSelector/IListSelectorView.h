#pragma once

#include <vector>

#include "Model/AmbListRow.h"

class IListSelectorView
{
public:
	virtual void setDates(const Date& from, const Date& to) = 0;
	virtual void setRows(const std::vector<AmbListRow>& rows) = 0;
	virtual void focus() = 0;
	virtual void close() = 0;
};