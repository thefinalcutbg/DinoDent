#include "UnusedPackageView.h"

#include <QFileDialog>
#include <QPainter>
#include "View/GlobalFunctions.h"

UnusedPackageView::UnusedPackageView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.csvButton->setIcon(QIcon(":/icons/icon_csv.png"));
	ui.pisCheck->setIcon(QIcon(":/icons/icon_nhif.png"));
	ui.nraCheck->setIcon(QIcon(":/icons/icon_nra.png"));


	connect(ui.button, &QPushButton::clicked, this, [&] {

		auto date = ui.lastVisitCheck->isChecked() ? ui.dateEdit->getDate() : Date();	
		presenter.buttonPressed(
			UnusedPackageSettings{
				.excludeBefore = date,
				.pisCheckEnabled = ui.pisCheck->isChecked(),
				.nraCheckEnabled = ui.nraCheck->isChecked(),
				.nhifCurrentDentistOnly = ui.excludeNonNhifCheck->isChecked()
			}
		);
	});

	connect(ui.dateEdit, &QDateEdit::dateChanged, this, [&] {
		presenter.resetQueue();
		ui.tableWidget->setRowCount(0);
		ui.progressBar->setMaximum(1);
		ui.progressBar->setValue(0);
		ui.progressBar->setTextVisible(false);
	});


	connect(ui.tableWidget, &QTableWidget::doubleClicked, this,
		[&](const QModelIndex& index) {
			presenter.newAmbList(ui.tableWidget->item(index.row(), 0)->data(0).toLongLong());
	});

	connect(ui.lastVisitCheck, &QCheckBox::stateChanged, this, [&] {
		ui.dateEdit->setDisabled(!ui.lastVisitCheck->isChecked());
	});

	connect(ui.csvButton, &QPushButton::clicked, this, [&] { exportToCSV();});

	ui.tableWidget->hideColumn(0);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->setColumnWidth(4, 150);

	presenter.setView(this);

	ui.csvButton->setDisabled(!ui.tableWidget->rowCount());
	
}

void UnusedPackageView::addRow(const PackageRowData& row)
{
	ui.tableWidget->setSortingEnabled(false);

	QTableWidgetItem* item;

	int rowIdx = ui.tableWidget->rowCount();

	ui.tableWidget->insertRow(rowIdx);

	ui.tableWidget->setItem(rowIdx, 0,
		new QTableWidgetItem(QString::number(row.rowid))
	);

	item = new QTableWidgetItem(row.patientName.c_str());

	//indicator
	auto indicator_color = QColor(row.color_indicator.c_str());

	if(indicator_color.isValid()){
		QPixmap px(64, 64);

		px.fill(Qt::transparent);

		QPainter painter(&px);

		painter.setPen(Qt::PenStyle::NoPen);	

		painter.setBrush(indicator_color);
		painter.drawEllipse(QRect(10, 10, 44, 44));
		item->setIcon(QIcon(px));
	}
	else {
		item->setIcon(QIcon());
	}

	ui.tableWidget->setItem(rowIdx, 1, item);

	QString age = row.age < 10 ? 
		" " + QString::number(row.age)
		: 
		QString::number(row.age);
	
	item = new QTableWidgetItem(age);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 2, item);

	item = new QTableWidgetItem(row.patientPhone.c_str());
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 3, item);

	item = new QTableWidgetItem(row.lastVisit.c_str());
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 4, item);

	item = new QTableWidgetItem(row.exam.c_str());
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 5, item);

	QString package = QString::number(row.procedure_count) + "/";
	package += QString::number(row.procedure_max);

	item = new QTableWidgetItem(package);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 6, item);

	item = new QTableWidgetItem(row.upperDenture.c_str());
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 7, item);

	item = new QTableWidgetItem(row.lowerDenture.c_str());
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(rowIdx, 8, item);

	ui.tableWidget->scrollToBottom();

	ui.tableWidget->setSortingEnabled(true);
}

void UnusedPackageView::addTable(const std::vector<PackageRowData>& table)
{
	ui.tableWidget->setRowCount(0);

	for (auto& row : table) {
		addRow(row);
	}
}


void UnusedPackageView::setProgressCount(int count)
{
	ui.progressBar->setValue(0);
	ui.progressBar->setMaximum(count);
	ui.progressBar->setTextVisible(true);
	ui.button->setText("Спри");
	ui.csvButton->setDisabled(true);
	ui.dateEdit->setDisabled(true);
	ui.pisCheck->setDisabled(true);
	ui.nraCheck->setDisabled(true);
	ui.excludeNonNhifCheck->setDisabled(true);
	ui.lastVisitCheck->setDisabled(true);
}


void UnusedPackageView::increment()
{
	ui.progressBar->setValue(ui.progressBar->value()+1);
}

void UnusedPackageView::setSumLabel(double price)
{
	QApplication::processEvents();

	if (!price) {
		ui.sumLabel->clear();
		return;
	}

	ui.sumLabel->setText("Обща сума: " + formatDoubleWithDecimal(price) +" €");
}

void UnusedPackageView::setSettings(const UnusedPackageSettings& settings)
{
	ui.pisCheck->setChecked(settings.pisCheckEnabled);
	ui.nraCheck->setChecked(settings.nraCheckEnabled);
	ui.excludeNonNhifCheck->setChecked(settings.nhifCurrentDentistOnly);
	ui.lastVisitCheck->setChecked(!settings.excludeBefore.isDefault());

	if (!settings.excludeBefore.isDefault()) {
		ui.dateEdit->set_Date(settings.excludeBefore);
	}
}

void UnusedPackageView::reset()
{
	ui.button->setText("Генерирай списък");
	ui.csvButton->setDisabled(!ui.tableWidget->rowCount());
	ui.dateEdit->setDisabled(!ui.lastVisitCheck->isChecked());
	ui.pisCheck->setDisabled(false);
	ui.nraCheck->setDisabled(false);
	ui.excludeNonNhifCheck->setDisabled(false);
	ui.lastVisitCheck->setDisabled(false);
}

void UnusedPackageView::exportToCSV()
{
	if (!ui.tableWidget->rowCount()) return;

	QString filename = QFileDialog::getSaveFileName(
		this, "Запази като CSV", "patients.csv", "CSV files (*.csv)"
	);

	QFile data(filename);

	if (data.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream output(&data);

		auto cCount = ui.tableWidget->columnCount();
		auto rCount = ui.tableWidget->rowCount();

		for (int r = 0; r < rCount; r++) {

			for (int c = 1; c < PackageRowData::columnCount; c++) {

				output << ui.tableWidget->item(r, c)->data(0).toString();
				
				if (c == cCount - 1) break;
				
				output << ",";
			}

			output << "\n";
		}
	}
}

UnusedPackageView::~UnusedPackageView()
{}
