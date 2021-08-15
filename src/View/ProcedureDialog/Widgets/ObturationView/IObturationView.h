#pragma once
#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractSurfaceSelector.h"

class ObturData;

class IObturationView
{
public:
	virtual AbstractLineEdit* material() = 0;
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual std::tuple<int, bool> getColor() = 0;
	virtual bool hasPost() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const PObturationData& data) = 0;
	virtual PObturationData getData() = 0;
};

