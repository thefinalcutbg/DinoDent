#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"

class ProcedureObtData;

class IObturationView
{
public:
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const ProcedureObtData& data) = 0;
	virtual ProcedureObtData getData() = 0;
};

