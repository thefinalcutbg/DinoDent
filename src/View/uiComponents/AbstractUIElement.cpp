#include "AbstractUIElement.h"

AbstractUIElement::AbstractUIElement() :
	valid_State(1),
	validator(NULL)
{
}

void AbstractUIElement::set_Validator(Validator* validator)
{
	this->validator = validator;
	//if (validator == NULL) {
    //	}
	setAppearence(true); //not sure about this?
}
void AbstractUIElement::forceValidate()
{
		if (validator == NULL)
		{
			valid_State = true;
			setAppearence(valid_State);
			return;
		}
		valid_State = (validationOperation());
		setAppearence(valid_State);
}
bool AbstractUIElement::isValid()
{
	return valid_State; 
}


