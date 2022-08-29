#include "AbstractUIElement.h"


AbstractUIElement::AbstractUIElement() :
	valid_State(1),
	validator(nullptr)
{
}

void AbstractUIElement::setInputValidator(Validator* validator)
{
	this->validator = validator;
	
	setValidAppearence(true); //not sure about this?
}
void AbstractUIElement::validateInput()
{
	valid_State = validator ? validationOperation() : true;

	setValidAppearence(valid_State);
}
bool AbstractUIElement::isValid()
{
	return valid_State; 
}


