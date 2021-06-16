#include "IdLineEdit.h"

IdLineEdit::IdLineEdit(QWidget* parent) : LineEdit(parent)
{

}

void IdLineEdit::stateChangedByUser()
{
	forceValidate();

	if (isValid()) emit validIdEntered();
}

