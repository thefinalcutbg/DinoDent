#include "CalendarEventDialog.h"

CalendarEventDialog::CalendarEventDialog(const CalendarEvent& event, QWidget *parent) : 
	m_result(event),
	QDialog(parent)
{
	ui.setupUi(this);

	ui.summaryEdit->setText(event.summary.c_str());
	ui.startDateTimeEdit->setDateTime(event.start);
	ui.endDateTimeEdit->setDateTime(event.end);

	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		m_result.summary = ui.summaryEdit->text().toStdString();
		m_result.start = ui.startDateTimeEdit->dateTime();
		m_result.end = ui.endDateTimeEdit->dateTime();

		accept();

	});

	ui.summaryEdit->setFocus();
	ui.summaryEdit->setCursorPosition(ui.summaryEdit->text().size());
}

CalendarEventDialog::~CalendarEventDialog()
{}