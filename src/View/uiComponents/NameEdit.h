#pragma once
#include "LineEdit.h"

class NameEdit : public LineEdit
{


	QString	reformat(QString text);
	void makeFirstLetterCapital();
	void reformat() override;

public:
	NameEdit(QWidget* parent = 0);

private:
	Q_OBJECT
};

