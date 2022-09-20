#include "DateEdit.h"
#include <QLabel>

DateEdit::DateEdit(QWidget* parent) : CustomDateEdit(parent), errorLabel(nullptr)
{
	connect(this, &QDateEdit::dateChanged, [=] { AbstractUIElement::validateInput(); });
}


void DateEdit::setValidAppearence(bool valid)
{
	/*
	if (valid)
		setStyleSheet("");
	else
		setStyleSheet("border: 1px solid red;");
		*/
	if (errorLabel == nullptr) return;

	valid ?
		errorLabel->setText("")
		:
		errorLabel->setText(QString::fromStdString(AbstractUIElement::validator->getErrorMessage()));
}


void DateEdit::setFocus()
{
	QWidget::setFocus();
}

void DateEdit::set_Date(const Date& date)
{
	QSignalBlocker blocker(this);
	QDateEdit::setDate(QDate(date.year, date.month, date.day));

}

Date DateEdit::getDate()
{
	auto qdate = date();
	return Date(qdate.day(), qdate.month(), qdate.year());
}

void DateEdit::reset()
{
	QDateEdit::setDate(minimumDate());
	setValidAppearence(true);
}

void DateEdit::disable(bool disable)
{
}

void DateEdit::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}
