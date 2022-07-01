#include "ReportDialog.h"
#include <QFileDialog>
#include <QPainter>
#include "Presenter/ReportDialogPresenter/ReportDialogPresenter.h"

ReportDialog::ReportDialog(ReportDialogPresenter* p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setModal(true);

	setWindowTitle("Генериране на отчет");
	setWindowIcon(QIcon(":/icons/icon_reports.png"));

	constexpr const char* monthNames[12]
	{
		u8"Януари",
		u8"Февруари",
		u8"Март",
		u8"Април",
		u8"Май",
		u8"Юни",
		u8"Юли",
		u8"Август",
		u8"Септември",
		u8"Октомври",
		u8"Ноември",
		u8"Декември"
	};

	for (int i = 0; i < 12; i++)
		ui.monthCombo->addItem(monthNames[i]);

	connect(ui.generateButton, &QPushButton::clicked, [=] {
		
		m_stop? 
		presenter->reset()
		:
		presenter->generateReport(
			ui.pisCheck->isChecked(),
			ui.nraCheck->isChecked()
			);

	});

	connect(ui.monthCombo, &QComboBox::currentIndexChanged, [=](int idx) {
		presenter->setDate(ui.monthCombo->currentIndex() + 1, ui.yearSpin->value());
		});

	connect(ui.yearSpin, &QSpinBox::valueChanged, [=](int value) {
		presenter->setDate(ui.monthCombo->currentIndex() + 1, ui.yearSpin->value());
		});

	connect(ui.pisButton, &QPushButton::clicked, [=] { presenter->sendToPis(); });
	connect(ui.xmlButton, &QPushButton::clicked, [=] { presenter->saveToXML(); });

	presenter->setView(this);

	ui.monthCombo->setCurrentIndex(Date::currentMonth() - 1); // index 0 == january;
	ui.yearSpin->setValue(Date::currentYear());

	
}


void ReportDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}


ReportDialog::~ReportDialog()
{
}

void ReportDialog::appendText(const std::string& text)
{
	ui.textEdit->append(text.data());
}

void ReportDialog::clearText()
{
	ui.textEdit->clear();
}

void ReportDialog::setPercent(int percent)
{
	ui.progressBar->setValue(percent);
}

void ReportDialog::enableReportButtons(bool enabled)
{
	ui.pisButton->setEnabled(enabled);
	ui.xmlButton->setEnabled(enabled);

}

void ReportDialog::showStopButton(bool yes)
{
	yes ?
		ui.generateButton->setText(u8"Спри")
		:
		ui.generateButton->setText(u8"Генерирай отчет");

		m_stop = yes;

		ui.nraCheck->setDisabled(yes);
		ui.pisCheck->setDisabled(yes);
		ui.monthCombo->setDisabled(yes);
		ui.yearSpin->setDisabled(yes);
}
