#include "AbstractUIElement.h"


AbstractUIElement::AbstractUIElement() :
	valid_State(1),
	validator(nullptr)
{
}

void AbstractUIElement::setInputValidator(Validator* validator)
{
	this->validator = validator;
	//if (validator == NULL) {
    //	}
	setValidAppearence(true); //not sure about this?
}
void AbstractUIElement::validateInput()
{
		if (validator == NULL)
		{
			valid_State = true;
			setValidAppearence(valid_State);
			return;
		}
		valid_State = (validationOperation());
		setValidAppearence(valid_State);
}
bool AbstractUIElement::isValid()
{
	return valid_State; 
}


