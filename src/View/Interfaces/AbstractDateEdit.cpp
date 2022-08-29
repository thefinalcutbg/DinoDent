#include "AbstractDateEdit.h"

bool AbstractDateEdit::validationOperation()
{
	return validator->validateInput(getDate());
}

void AbstractDateEdit::reset()
{
	set_Date(Date{ 1, 1, 1900 });
}
