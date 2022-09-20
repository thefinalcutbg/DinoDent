#include "AbstractLineEdit.h"


AbstractLineEdit::AbstractLineEdit() {};


void AbstractLineEdit::reset()
{
	set_Text("");
	setValidAppearence(true);
}


bool AbstractLineEdit::validationOperation()
{
	return validator->validateInput(getText());
}

