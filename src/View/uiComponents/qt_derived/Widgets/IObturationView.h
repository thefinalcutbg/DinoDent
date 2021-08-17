#pragma once
#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractSurfaceSelector.h"

class ProcedureObtData;

class IObturationView
{
public:
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const ProcedureObtData& data) = 0;
	virtual ProcedureObtData getData() = 0;
};

