#pragma once

#include <vector>

#include "Model/TableRows.h"
#include "Model/PlainTable.h"

struct BrowserUiState;

class IBrowserDialog
{
public:
	virtual void setUiState(const BrowserUiState& state) = 0;

	virtual void setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn) = 0;
	virtual void setPreview(const PlainTable& t) = 0;
	virtual void focus() = 0;
	virtual void close() = 0;
};