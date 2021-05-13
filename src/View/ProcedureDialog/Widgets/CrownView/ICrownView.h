#pragma once

#include "View/uiComponents/AbstractRangeEdit.h"
#include "View/uiComponents/AbstractLineEdit.h"

class ICrownView
{
public:
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual AbstractLineEdit* materialEdit() = 0;
	virtual int getType() = 0;
	virtual std::tuple<int, bool> getColor() = 0;
	virtual void set_hidden(bool hidden) = 0;
};