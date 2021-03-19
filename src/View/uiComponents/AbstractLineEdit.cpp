#include "AbstractLineEdit.h"


AbstractLineEdit::AbstractLineEdit() : 
	reformator(NULL), 
	observer(NULL) {};


void AbstractLineEdit::reset()
{
	setFieldText("");
	setAppearence(true);
}

void AbstractLineEdit::reformat()
{
	if (reformator != NULL)
	{
		QString currentText = getText().c_str();
		QString newText = reformator->reformat(currentText);
		setFieldText(newText.toStdString());
	}
}

bool AbstractLineEdit::validationOperation()
{
	return validator->validate(getText());
}


void AbstractLineEdit::setReformator(Reformator* reformator)
{
	this->reformator = reformator;
}

void AbstractLineEdit::setOberver(uiObserver* observer)
{
	this->observer = observer;
}
