#pragma once

#include "View/Interfaces/AbstractRangeEdit.h"
#include "Model/Dental/StatusData.h"

struct ProcedureFiberData;

class IFiberSplintView
{
public:
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const ProcedureFiberData& data) = 0;
	virtual ObturationData getData() = 0;

};