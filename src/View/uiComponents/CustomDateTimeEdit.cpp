#include "CustomDateTimeEdit.h"
#include "CalendarWidget.h"
#include <QLineEdit>

CustomDateTimeEdit::CustomDateTimeEdit(QWidget* parent) : QDateTimeEdit(parent)
{
    setDate(QDate::currentDate());
    setDisplayFormat("dd.MM.yyyy HH:mm");
    setLocale(QLocale(QLocale::Bulgarian));
    setCalendarPopup(true);
    setCalendarWidget(new CalendarWidget(this));
    lineEdit()->setFrame(false);
}

CustomDateTimeEdit::~CustomDateTimeEdit()
{

}

