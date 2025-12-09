#include "CalendarEventDialog.h"

#include "Database/DbPatient.h"

#include "Model/User.h"
#include "Model/FreeFunctions.h"

#include "Network/SMS/SMSMessage.h"
#include "Network/SMS/Mobica.h"

#include <QCompleter>
#include <QPainter>
#include <QAbstractItemView>
#include <unordered_map>

struct CompleterData {
	std::string fname;
	std::string birth;
	std::string phone;
};

std::unordered_map<QString, CompleterData> s_completer;

CalendarEventDialog::CalendarEventDialog(const CalendarEvent& event, QWidget *parent) :
	m_result(event),
	QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(event.id.size() ?
		"Редакция на посещение"
		:
		"Ново посещение"
	);
	
	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		QString summary = ui.summaryEdit->text();

		m_result.summary = summary.toStdString();
		m_result.description = ui.descriptionEdit->text().toStdString();
		m_result.start = ui.startDateTimeEdit->dateTime();
		m_result.end = ui.endDateTimeEdit->dateTime();
		
		if(s_completer.count(summary))
		{
			auto& data = s_completer[summary];
			m_result.patientFname = data.fname;
			m_result.patientBirth = data.birth;
		}

		smsLogic();

		accept();

	});

	connect(ui.summaryEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		ui.iconLabel->setText(s_completer.count(text) ?
			"<font color=\"Green\">✓</font>" : ""
		);

		if(ui.startDateTimeEdit->dateTime() < QDateTime::currentDateTime())
		{
			ui.smsFrame->setHidden(true);
			return;
		}

		m_phone = FreeFn::getPhoneFromString(text.toStdString()).c_str();

		ui.smsFrame->setHidden(m_phone.isEmpty() || !User::settings().sms_settings.hasCredentials());
	});

	connect(ui.smsReminderSpin, &QSpinBox::valueChanged, this, [&](int value) {
		ui.smsReminderSpin->setSuffix(value == 1 ? " час" : " часа");

		ui.reminderTimeLabel->setText(
			"Време на напомнянето: " +
			ui.startDateTimeEdit->dateTime().addSecs(-value * 60 * 60).toString("dd.MM.yyyy HH:mm")
		);

	});

	connect(ui.smsReminderCheck, &QCheckBox::toggled, this, [&](bool checked) {
			ui.smsReminderSpin->setHidden(!checked);
			ui.label_6->setHidden(!checked);
			ui.reminderTimeLabel->setHidden(!checked);
	});

	ui.smsFrame->setHidden(true);
	ui.smsReminderSpin->setHidden(true);
	ui.label_6->setHidden(true);
	ui.reminderTimeLabel->hide();

	//setting autocomplete patients

	s_completer.clear();

	QStringList completerList;

	for (auto p : DbPatient::getPatientList())
	{
		QString summary = QString::fromStdString(p.summary);

		completerList.push_back(summary);
		s_completer[summary] = { p.fname, p.birth, p.phone};
	}

	auto new_completer = new QCompleter(completerList, this);
	new_completer->setCaseSensitivity(Qt::CaseInsensitive);
	new_completer->setCompletionMode(QCompleter::PopupCompletion);

	QFont f;
	//f.setPixelSize(10);
	new_completer->popup()->setFont(f);
	new_completer->setMaxVisibleItems(10);
	new_completer->setModelSorting(QCompleter::UnsortedModel);
	ui.summaryEdit->setCompleter(new_completer);

	//important to set before setting text because of sms section show/hide logic
	ui.startDateTimeEdit->setDateTime(event.start);
	ui.endDateTimeEdit->setDateTime(event.end);

	ui.summaryEdit->setText(event.summary.c_str());
	ui.descriptionEdit->setText(event.description.c_str());


	ui.summaryEdit->setFocus();

	if (event.summary.size()) {
		ui.descriptionEdit->setFocus();
	}

	//default is 0. We ensure change and signal emition
	ui.smsReminderSpin->setValue(User::settings().sms_settings.reminder_hours);
	ui.smsReminderSpin->setMinimum(1);
}

void CalendarEventDialog::smsLogic()
{
	if (!ui.smsFrame->isVisible()) return;

	if (!User::settings().sms_settings.hasCredentials()) return;

	std::vector<SMSMessage> smsMessages;

	if (ui.smsNotifyCheck->isChecked()) {
		smsMessages.emplace_back(
			m_phone.toStdString(),
			User::settings().sms_settings.notifTemplate,
			m_result.start.toString(SMSMessage::expectedFormat).toStdString()
		);
	}

	if (ui.smsReminderCheck->isChecked())
	{
		smsMessages.emplace_back(
			m_phone.toStdString(),
			User::settings().sms_settings.reminderTemplate,
			m_result.start.toString(SMSMessage::expectedFormat).toStdString(),
			ui.smsReminderSpin->value()
		);
	}

	static Mobica::SendSMS sms_service;

	sms_service.sendSms(smsMessages);
		
}

void CalendarEventDialog::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

CalendarEventDialog::~CalendarEventDialog()
{}
