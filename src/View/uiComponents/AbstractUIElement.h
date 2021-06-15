#pragma once
#include "Model/Validator/AbstractValidator.h"

class AbstractUIElement
{
	
	bool valid_State;
protected:
	Validator* validator;

	virtual void stateChangedByUser() { forceValidate(); }

	/*
	Override the validation depeneding on which type of data you want to validate.
	If you want to add some more data, add an empry virtual function in AbstractValidator class
	which returns true, and override the function in concrete validator. Then use it here.
	Never call the function directly! Use forceValidate() instead*/
	virtual bool validationOperation() = 0;
public:
	AbstractUIElement();
	virtual void setFocusAndSelectAll() = 0;
	virtual void disable(bool disable) = 0;
	virtual void setAppearence(bool valid) = 0;
	void set_Validator(Validator* validator);

	virtual void forceValidate();
	bool isValid();
public:
};

