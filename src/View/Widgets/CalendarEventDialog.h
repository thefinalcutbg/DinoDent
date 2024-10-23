#pragma once

#include <QDialog>
#include "ui_CalendarEventDialog.h"
#include "Model/CalendarStructs.h"

class CalendarEventDialog : public QDialog
{
	Q_OBJECT

	CalendarEvent m_result;

	void paintEvent(QPaintEvent* e) override;

public:
	CalendarEventDialog(const CalendarEvent& event, QWidget *parent = nullptr);
	CalendarEvent& result() { return m_result; }
	~CalendarEventDialog();

private:
	Ui::CalendarEventDialogClass ui;
};
