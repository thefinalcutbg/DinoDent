#include "IdLineEdit.h"

IdLineEdit::IdLineEdit(QWidget* parent) : LineEdit(parent)
{

}

void IdLineEdit::stateChangedByUser()
{
	forceValidate();

	if (isValid() && observer != NULL)
		observer->notify(Notification::validID);
}

