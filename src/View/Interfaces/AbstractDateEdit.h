#pragma once

#include "AbstractUIElement.h"
#include "Model/Date.h"

class AbstractDateEdit : public AbstractUIElement
{
	bool validationOperation() override;

public:

	virtual void set_Date(const Date& date) = 0;
	virtual Date getDate() = 0;

	virtual void reset();

};