#pragma once

#include "View/uiComponents/AbstractLineEdit.h"
#include <tuple>

class ImplantData;

class IImplantView
{
public:

	virtual void set_hidden(bool hidden) = 0;
	virtual ImplantData getData() = 0;
	virtual void setData(const ImplantData& data) = 0;
};