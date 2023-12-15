#pragma once

#include <vector>

#include "Model/TableRows.h"
#include "Model/PlainTable.h"

class IBrowserDialog
{
public:
	virtual void setDates(const Date& from, const Date& to) = 0;

	virtual void setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn) = 0;
	virtual void setPreview(const PlainTable& t) = 0;
	virtual void focus() = 0;
	virtual void close() = 0;
};