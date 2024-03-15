#include "DateEdit.h"
#include <QLabel>
#include <QLineEdit>
#include "CalendarWidget.h"

DateEdit::DateEdit(QWidget* parent) : QDateEdit(parent), errorLabel(nullptr)
{
    setDate(QDate::currentDate());

    connect(this, &QDateEdit::dateChanged, [&] { AbstractUIElement::validateInput(); });

    setCalendarPopup(true);
    auto c = new CalendarWidget;
    setCalendarWidget(c);
    setDisplayFormat("dd.MM.yyyy");
    setLocale(QLocale(QLocale::Bulgarian));
    lineEdit()->setFrame(false);
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


void DateEdit::set_focus()
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

void DateEdit::disable(bool)
{
}

void DateEdit::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}
