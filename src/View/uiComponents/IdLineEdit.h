#pragma once
#include "LineEdit.h"

class IdLineEdit : public LineEdit
{
	Q_OBJECT

protected:
	void stateChangedByUser() override;
public:
	IdLineEdit(QWidget* parent = 0);
};

