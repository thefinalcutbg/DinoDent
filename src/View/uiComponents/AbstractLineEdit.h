#pragma once
#include "Model/Validator/AbstractValidator.h"
#include "AbstractUIElement.h"
#include <string>


class AbstractLineEdit : public AbstractUIElement
{
	
protected:

	virtual void reformat() {};
	bool validationOperation() override;

public:
	AbstractLineEdit();

	virtual void setFieldText(const std::string& text) = 0;
	virtual std::string getText() = 0;

	virtual void reset();
};