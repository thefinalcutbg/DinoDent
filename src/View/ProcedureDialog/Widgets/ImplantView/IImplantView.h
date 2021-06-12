#pragma once

#include "View/uiComponents/AbstractLineEdit.h"

class IImplantView
{
public:

	virtual void set_hidden(bool hidden) = 0;
	virtual AbstractLineEdit* systemEdit() = 0;
};