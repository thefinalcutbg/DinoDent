#include "AbstractLineEdit.h"


AbstractLineEdit::AbstractLineEdit() {};


void AbstractLineEdit::reset()
{
	setFieldText("");
	setAppearence(true);
}


bool AbstractLineEdit::validationOperation()
{
	return validator->validate(getText());
}

