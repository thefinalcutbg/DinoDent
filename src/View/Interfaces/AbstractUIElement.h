#pragma once
#include "Model/Validators/AbstractValidator.h"


class AbstractUIElement
{

	bool valid_State;
protected:
	Validator* validator;

	/*
	Override the validation operation depeneding on which type of data you want to validate.
	If you want to add some more data, add an empty virtual function in AbstractValidator class
	which returns true, and override the function in concrete validator. Then use it here.
	Never call the function directly! Use forceValidate() instead*/
	virtual bool validationOperation() = 0;
public:
	AbstractUIElement();
    virtual void set_focus() = 0;
    virtual void disable(bool) {};
    virtual void hide(bool) {};

    virtual void setValidAppearence(bool) {};
	Validator* getValidator() { return validator; };
	void setInputValidator(Validator* validator);
	virtual bool validateInput();
	bool isValid();
public:
};
