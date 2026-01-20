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
	std::string email;
};

const QString s_emailPrefix = "🦷 ";

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
	
	//removing the email prefix if present
	QString summary = event.summary.c_str();

	if(summary.startsWith(s_emailPrefix))
	{
		summary = summary.mid(s_emailPrefix.length());
		m_result.summary = summary.toStdString();
		m_prefixRemoved = true;
	}
	
	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		QString summary = ui.summaryEdit->text();

		m_result.summary = summary.toStdString();

		if (m_prefixRemoved) {
			//adding the email prefix
			summary = s_emailPrefix + summary;
			m_result.summary = summary.toStdString();
		}

		m_result.description = ui.descriptionEdit->text().toStdString();
		m_result.start = ui.startDateTimeEdit->dateTime();
		m_result.end = ui.endDateTimeEdit->dateTime();
		
		if(s_completer.count(summary))
		{
			auto& data = s_completer[summary];
			m_result.patientFname = data.fname;
			m_result.patientBirth = data.birth;
			
			if ((ui.emailCheck->isVisible() && ui.emailCheck->isChecked())) {
				m_result.email = data.email;
				//adding the email prefix
				summary = s_emailPrefix + summary;
				m_result.summary = summary.toStdString();
			}
		}

		smsLogic();

		accept();

	});

	connect(ui.summaryEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		bool hasCompletion = s_completer.count(text);

		ui.iconLabel->setText(hasCompletion ? "<font color=\"Green\">✓</font>" : "");

		m_phone = FreeFn::getPhoneFromString(text.toStdString()).c_str();

		emailShowLogic();

		smsFrameShowLogic();
	});

	connect(ui.smsReminderSpin, &QSpinBox::valueChanged, this, [&](int value) { updateReminderTimeLabel(); });

	connect(ui.startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [&](const QDateTime& dateTime) {
		smsFrameShowLogic();
		emailShowLogic();
		updateReminderTimeLabel();
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
		s_completer[summary] = { p.fname, p.birth, p.phone, p.email};
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
	ui.startDateTimeEdit->setDateTime(m_result.start);
	ui.endDateTimeEdit->setDateTime(m_result.end);
	//set the m_summary with the removed email prefix
	ui.summaryEdit->setText(m_result.summary.c_str());
	ui.descriptionEdit->setText(m_result.description.c_str());


	ui.summaryEdit->setFocus();

	if (m_result.summary.size()) {
		ui.descriptionEdit->setFocus();
	}

	//default is 0. We ensure change and signal emition
	ui.smsReminderSpin->setValue(User::settings().sms_settings.reminder_hours);
	ui.smsReminderSpin->setMinimum(1);

	if (m_result.id.empty()) {
		ui.smsNotifyCheck->setChecked(User::settings().sms_settings.notifAuto);
		ui.smsReminderCheck->setChecked(User::settings().sms_settings.reminderAuto);
	}
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

void CalendarEventDialog::smsFrameShowLogic()
{
	if (!User::settings().sms_settings.hasCredentials()) {
		ui.smsFrame->hide();
		return;
	}

	if (m_phone.isEmpty()) {
		ui.smsFrame->hide();
		return;
	}

	if (ui.startDateTimeEdit->dateTime() < QDateTime::currentDateTime()) {
		ui.smsFrame->hide();
		return;
	}

	auto reminder_secs = ui.smsReminderSpin->value() * 60 * 60;

	if (QDateTime::currentDateTime().addSecs(reminder_secs) > ui.startDateTimeEdit->dateTime()) {
		ui.smsReminderCheck->setChecked(false);
	}

	ui.smsFrame->show();
}

void CalendarEventDialog::updateReminderTimeLabel()
{
	ui.smsReminderSpin->setSuffix(ui.smsReminderSpin->value() == 1 ? " час" : " часа");

	auto reminderTime = ui.startDateTimeEdit->dateTime().addSecs(-ui.smsReminderSpin->value() * 60 * 60);

	ui.reminderTimeLabel->setStyleSheet(
		reminderTime < QDateTime::currentDateTime() ? "color: darkRed" : ""
	);

	ui.reminderTimeLabel->setText(
		"Време на напомнянето: " +
		reminderTime.toString("dd.MM.yyyy HH:mm")
	);
}

void CalendarEventDialog::emailShowLogic()
{
	if (!s_completer.count(ui.summaryEdit->text()))
	{
		ui.emailCheck->hide();
		return;
	}

	if (m_result.id.size()) {
		ui.emailCheck->hide();
		return;
	}

	if(ui.startDateTimeEdit->dateTime() < QDateTime::currentDateTime())
	{
		ui.emailCheck->hide();
		return;
	}

	if (s_completer.at(ui.summaryEdit->text()).email.empty())
	{
		ui.emailCheck->hide();
		return;
	}

	ui.emailCheck->setChecked(true);

	ui.emailCheck->show();
}

CalendarEventDialog::~CalendarEventDialog()
{}
