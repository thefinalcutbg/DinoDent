#pragma once
#include "LineEdit.h"

class NameEdit : public LineEdit
{

	static QString letters; //= "абвгдежзийклмнопрстуфхцчшщъьюя-";
	static QString capletters; //= "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯ ";

	QString	reformat(QString text);
	void makeFirstLetterCapital();
	void reformat() override;

public:
	NameEdit(QWidget* parent = 0);

private:
	Q_OBJECT
};

