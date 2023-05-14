#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"

struct RestorationData;

class IObturationView
{
public:
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const RestorationData& data) = 0;
	virtual RestorationData getData() = 0;
};

