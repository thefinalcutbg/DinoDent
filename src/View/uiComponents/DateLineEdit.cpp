#include "DateLineEdit.h"

DateLineEdit::DateLineEdit(QWidget* parent) : QDateEdit(parent)
{
	connect(this, &QDateEdit::dateChanged, [=] { qDebug() << "changed by user"; stateChangedByUser(); });

}

void DateLineEdit::setFocusAndSelectAll()
{
	setFocus();
}

void DateLineEdit::setAppearence(bool valid)
{
	if (valid)
		setStyleSheet("");
	else
		setStyleSheet("border: 1px solid red;");
}

void DateLineEdit::stateChangedByUser() 
{ 
	forceValidate(); 
	if (observer != NULL)
	{
		observer->notify(Notification::edited);
	}
	
};

void DateLineEdit::setFieldText(const std::string& text)
{
	blockSignals(true);
	if (text == "") setDate(minimumDate());
	setDate(QDate::fromString(QString::fromStdString(text), "dd.MM.yyyy"));
	blockSignals(false);
}

std::string DateLineEdit::getText()
{
	return date().toString("dd.MM.yyyy").toStdString();
}

void DateLineEdit::reset()
{
	setDate(minimumDate());
}

void DateLineEdit::disable(bool disable)
{
}
