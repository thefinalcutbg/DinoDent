#include "UnusedPackageView.h"

UnusedPackageView::UnusedPackageView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	auto date = Date::currentDate();
	date.year--;
	date.day = 1;
	date.month = 1;

	ui.dateEdit->set_Date(date);

	connect(ui.button, &QPushButton::clicked, this, [&] {
		presenter.buttonPressed(ui.dateEdit->getDate());
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

	ui.tableWidget->hideColumn(0);
	ui.tableWidget->setColumnWidth(1, 180);
	ui.tableWidget->setColumnWidth(3, 150);
	ui.tableWidget->setColumnWidth(4, 80);
}

void UnusedPackageView::addRow(const RowView& row)
{
	QTableWidgetItem* item;

	int rowIdx = ui.tableWidget->rowCount();

	ui.tableWidget->insertRow(rowIdx);

	ui.tableWidget->setItem(rowIdx, 0,
		new QTableWidgetItem(QString::number(row.rowid))
	);

	ui.tableWidget->setItem(rowIdx, 1,
		new QTableWidgetItem(row.patientName.c_str())
	);

	item = new QTableWidgetItem(row.patientPhone.c_str());
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 2, item);

	item = new QTableWidgetItem(row.maxDate.c_str());
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 3, item);

	item = new QTableWidgetItem(row.exam ? "Да" : "Не");
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 4, item);

	QString package = QString::number(row.procedure_count) + "/";
	package += QString::number(row.procedure_max);

	item = new QTableWidgetItem(package);
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 5, item);

	item = new QTableWidgetItem(row.upperDenture.c_str());
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 6, item);

	item = new QTableWidgetItem(row.lowerDenture.c_str());
	item->setTextAlignment(Qt::AlignCenter);

	ui.tableWidget->setItem(rowIdx, 7, item);

	ui.tableWidget->scrollToBottom();
}


void UnusedPackageView::setProgressCount(int count)
{
	ui.progressBar->setMaximum(count);
	ui.progressBar->setTextVisible(true);
	ui.button->setText("Спри");
	ui.dateEdit->setDisabled(true);
}


void UnusedPackageView::increment()
{
	ui.progressBar->setValue(ui.progressBar->value()+1);
}

void UnusedPackageView::reset()
{
	ui.button->setText("Генерирай списък");
	ui.dateEdit->setDisabled(false);

}

UnusedPackageView::~UnusedPackageView()
{}
