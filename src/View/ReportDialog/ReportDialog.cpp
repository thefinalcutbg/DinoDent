#include "ReportDialog.h"
#include <QFileDialog>
#include <QPainter>
ReportDialog::ReportDialog(std::optional<ReportDialogResult>& result, QWidget *parent)
	: ref_result(result), QDialog(parent)
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

	int currentMonth = Date::currentMonth();
	int previousMonth = currentMonth == 1 ? 12 : currentMonth - 1;

	int year = currentMonth == 1 ? Date::currentYear()-1 : Date::currentYear();

	ui.monthCombo->setCurrentIndex(previousMonth - 1); // index 0 == january;
	ui.yearSpin->setValue(year);
	ui.pathLineEdit->setText(QCoreApplication::applicationDirPath());

	connect(ui.okButton, &QPushButton::clicked, [&] {
			
		ref_result.emplace(ReportDialogResult
			{
			ui.monthCombo->currentIndex() + 1,
			ui.yearSpin->value(),
			ui.pathLineEdit->text().toStdString()
		});

		this->accept();
		
		});

	connect(ui.browseButton, &QPushButton::clicked, [&] {

			ui.pathLineEdit->setText(
				QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				"/home",
				QFileDialog::ShowDirsOnly
				| QFileDialog::DontResolveSymlinks));
		});
	
}


void ReportDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}


ReportDialog::~ReportDialog()
{
}
