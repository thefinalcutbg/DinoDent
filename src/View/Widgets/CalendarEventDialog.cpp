#include "CalendarEventDialog.h"
#include "Database/DbPatient.h"
#include "Model/User.h"
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

	ui.smsFrame->setHidden(true);
	ui.smsReminderSpin->setHidden(true);
	ui.label_6->setHidden(true);
	ui.smsReminderSpin->setValue(User::settings().sms_settings.reminder_hours);

	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		QString summary = ui.summaryEdit->text();

		m_result.summary = summary.toStdString();
		m_result.description = ui.descriptionEdit->text().toStdString();
		m_result.start = ui.startDateTimeEdit->dateTime();
		m_result.end = ui.endDateTimeEdit->dateTime();

		if (m_phone.size()) {

			auto& data = s_completer.at(summary);

			m_result.patientFname = data.fname;
			m_result.patientBirth = data.birth;

			if (!ui.smsFrame->isVisible()) return;

			if (ui.smsNotifyCheck) {
				m_smsMessages.push_back(
					SmsMessage{
					.phone = m_phone.toStdString(),
					.message = "Записано посещение при зъболекар на " +
					 ui.startDateTimeEdit->dateTime().toString("dd.MM.yyyy HH:mm").toStdString()
					}
				);
			}

			if(ui.smsReminderCheck->isChecked())
			{
				QDateTime remindTime = ui.startDateTimeEdit->dateTime().addSecs(-ui.smsReminderSpin->value() * 3600);
				
				m_smsMessages.push_back(
					SmsMessage{
					.phone = m_phone.toStdString(),
					.message = "Напомняне за посещение при зъболекар на " +
					 ui.startDateTimeEdit->dateTime().toString("dd.MM.yyyy HH:mm").toStdString(),
					.toDate = remindTime.toString("yyyy-MM-ddTHH:mm:ss").toStdString()
					}
				);
			}
		}

		sms_service.sendSms(m_smsMessages);

		accept();

	});

	connect(ui.summaryEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		if (s_completer.count(text)) 
		{
			ui.iconLabel->setText("<font color=\"Green\">✓</font>");

			m_phone = s_completer.at(text).phone.c_str();
		} 
		else 
		{
			ui.iconLabel->setText("");

			m_phone = "";
		}

		if(!User::settings().sms_settings.hasCredentials()) {
			m_phone = "";
			ui.smsFrame->setHidden(true);
			return;
		}

		const QRegularExpression re(R"((\+?\d[\d\s\-/]{8,}\d))");

		QRegularExpressionMatch match = re.match(text);

		if (m_phone.isEmpty() && match.hasMatch())
		{
			ui.smsFrame->setHidden(false);
			m_phone = match.captured(1);
			return;
			
		}

		ui.smsFrame->setHidden(m_phone.isEmpty());
	

	});

	connect(ui.smsReminderSpin, &QSpinBox::valueChanged, this, [&](int value) {
		ui.smsReminderSpin->setSuffix(value == 1 ? " час" : " часа");
	});

	connect(ui.smsReminderCheck, &QCheckBox::toggled, this, [&](bool checked) {
			ui.smsReminderSpin->setHidden(!checked);
			ui.label_6->setHidden(!checked);
		});

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

	ui.summaryEdit->setText(event.summary.c_str());
	ui.descriptionEdit->setText(event.description.c_str());
	ui.startDateTimeEdit->setDateTime(event.start);
	ui.endDateTimeEdit->setDateTime(event.end);

	ui.summaryEdit->setFocus();

	if (event.summary.size()) {
		ui.descriptionEdit->setFocus();
	}

}

void CalendarEventDialog::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

CalendarEventDialog::~CalendarEventDialog()
{}
