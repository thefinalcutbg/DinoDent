#include "CalendarEventDialog.h"

#include <QPainter>

CalendarEventDialog::CalendarEventDialog(const CalendarEvent& event, QWidget *parent) :
	m_result(event),
	QDialog(parent)
{
	setWindowTitle(event.id.size() ? 
		"Редакциа на посещение"
		:
		"Ново посещение"
	);

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

void CalendarEventDialog::paintEvent(QPaintEvent* e)
{
	QPainter p;
	p.fillRect(rect(), Qt::white);
}

CalendarEventDialog::~CalendarEventDialog()
{}
