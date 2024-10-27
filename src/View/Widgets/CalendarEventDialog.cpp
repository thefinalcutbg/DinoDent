#include "CalendarEventDialog.h"
#include "Database/DbPatient.h"
#include <QCompleter>
#include <QPainter>
#include <QAbstractItemView>
#include <unordered_map>

std::unordered_map<QString, std::pair<std::string, std::string>> s_completer;

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

		if (s_completer.count(summary)) {

			auto& [fname, birth] = s_completer.at(summary);

			m_result.patientFname = fname;
			m_result.patientBirth = birth;

		}

		accept();

	});

	connect(ui.summaryEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		ui.iconLabel->setText(
			s_completer.count(text) ?
			"<font color=\"Green\">✓</font>"
			:
			""
		);

	});

	//setting autocomplete patients

	s_completer.clear();

	QStringList completerList;

	for (auto p : DbPatient::getPatientList())
	{
		QString summary = QString::fromStdString(p.summary);

		completerList.push_back(summary);
		s_completer[summary] = std::make_pair(p.fname, p.birth);
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
