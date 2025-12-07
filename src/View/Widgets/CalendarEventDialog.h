#pragma once

#include <QDialog>
#include "ui_CalendarEventDialog.h"
#include "Model/CalendarStructs.h"
#include "Network/SMS/SMSMessage.h"
#include "Network/SMS/Mobica.h"

class CalendarEventDialog : public QDialog
{
	Q_OBJECT

	CalendarEvent m_result;

	QString m_phone;

	static inline Mobica::SmsReplyHandler sms_service;

	void paintEvent(QPaintEvent* e) override;

public:
	CalendarEventDialog(const CalendarEvent& event, QWidget *parent = nullptr);
	CalendarEvent& result() { return m_result; }
	~CalendarEventDialog();
	
	std::vector<SmsMessage> m_smsMessages;

private:
	Ui::CalendarEventDialogClass ui;
};
