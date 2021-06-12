#pragma once
#include "Model/Validator/AbstractValidator.h"
#include "Presenter/PatientDialog/uiObserver.h"
#include "AbstractUIElement.h"
#include <string>


class AbstractLineEdit : public AbstractUIElement
{
	
protected:
	uiObserver* observer;
	virtual void reformat() {};
	bool validationOperation() override;

public:
	AbstractLineEdit();

	virtual void setFieldText(const std::string& text) = 0;
	virtual std::string getText() = 0;

	void setOberver(uiObserver* observer);
	
	virtual void reset();
};