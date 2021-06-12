#pragma once

#include "View/uiComponents/AbstractLineEdit.h"
#include <tuple>
class IImplantView
{
public:

	virtual void set_hidden(bool hidden) = 0;
	virtual AbstractLineEdit* systemEdit() = 0;
	virtual std::tuple<std::string, int, int, double, double, int, int, bool, bool> getData() = 0;
};