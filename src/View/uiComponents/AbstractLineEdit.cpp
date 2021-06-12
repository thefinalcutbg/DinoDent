#include "AbstractLineEdit.h"


AbstractLineEdit::AbstractLineEdit() : 
	observer(nullptr) {};


void AbstractLineEdit::reset()
{
	setFieldText("");
	setAppearence(true);
}


bool AbstractLineEdit::validationOperation()
{
	return validator->validate(getText());
}


void AbstractLineEdit::setOberver(uiObserver* observer)
{
	this->observer = observer;
}
