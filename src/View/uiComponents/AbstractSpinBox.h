#pragma once
#include "AbstractUIElement.h"

class AbstractSpinBox : public AbstractUIElement
{
	bool validationOperation()  override { return true; }
public:

	virtual void set_Value(double value) = 0;
	virtual double get_Value() = 0;
};