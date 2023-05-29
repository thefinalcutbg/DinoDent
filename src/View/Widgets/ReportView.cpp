#include "ReportView.h"
#include <QFileDialog>
#include <QPainter>
#include "QtVersion.h"
#include <QDialog>
ReportView::ReportView(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);



	setWindowTitle("Генериране на отчет");
	setWindowIcon(QIcon(":/icons/icon_reports.png"));

	constexpr const char* monthNames[12]
	{
		"Януари",
		"Февруари",
		"Март",
		"Април",
		"Май",
		"Юни",
		"Юли",
		"Август",
		"Септември",
		"Октомври",
		"Ноември",
		"Декември"
	};

	for (int i = 0; i < 12; i++)
		ui.monthCombo->addItem(monthNames[i]);

	ui.textBrowser->setReadOnly(true);
	ui.textBrowser->setOpenLinks(false);

	connect(ui.generateButton, &QPushButton::clicked, [&] {

		m_stop ?
			presenter.reset()
			:
			presenter.generateReport(
				ui.pisCheck->isChecked(),
				ui.nraCheck->isChecked()
			);

		});

	connect(ui.monthCombo, QtComboIndexChanged, [&](int idx) {
		presenter.setDate(ui.monthCombo->currentIndex() + 1, ui.yearSpin->value());
		});

	connect(ui.yearSpin, QtSpinValueChanged, [&](int value) {
		presenter.setDate(ui.monthCombo->currentIndex() + 1, ui.yearSpin->value());
		});

	connect(ui.pisButton, &QPushButton::clicked, [&] { presenter.sendToPis(); });
	connect(ui.xmlButton, &QPushButton::clicked, [&] { presenter.saveToXML(); });

	connect(ui.textBrowser, &TextBrowser::linkPressed, [&](const QString& str) { presenter.linkClicked(str.toStdString()); });

	//CHANGE THIS LATER
	presenter.setView(this);

	ui.monthCombo->setCurrentIndex(Date::currentMonth() - 1); // index 0 == january;
	ui.yearSpin->setValue(Date::currentYear());

	//ui.textEdit->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

}


void ReportView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}


ReportView::~ReportView()
{
}

void ReportView::appendSheet(const std::string& numberLink, const std::string& description)
{
	QString line = "Амбулаторен лист № <a href=\"";
	line += numberLink.c_str();
	line += "\">";
	line += numberLink.c_str();
	line += "</a>: ";
	line += description.c_str();

	ui.textBrowser->append(line);
}

void ReportView::appendText(const std::string& text)
{
	ui.textBrowser->append(text.data());
}

void ReportView::clearText()
{
	ui.textBrowser->clear();
}

void ReportView::setPercent(int percent)
{
	ui.progressBar->setValue(percent);
}

void ReportView::enableReportButtons(bool enabled)
{
	ui.pisButton->setEnabled(enabled);
	ui.xmlButton->setEnabled(enabled);

}

void ReportView::showStopButton(bool yes)
{
	yes ?
		ui.generateButton->setText("Спри")
		:
		ui.generateButton->setText("Генерирай отчет");

		m_stop = yes;

		ui.nraCheck->setDisabled(yes);
		ui.pisCheck->setDisabled(yes);
		ui.monthCombo->setDisabled(yes);
		ui.yearSpin->setDisabled(yes);
}

void ReportView::closeDialog()
{
	static_cast<QDialog*>(parent()->parent()->parent())->close(); //laaaaaaaame
}
