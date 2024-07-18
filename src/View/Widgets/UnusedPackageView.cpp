#include "UnusedPackageView.h"

std::vector<QString> s_cache;
Date s_date;

UnusedPackageView::UnusedPackageView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	if (s_date == Date()) {
		s_date = Date::currentDate();
		s_date.year--;
		s_date.day = 1;
		s_date.month = 1;
	}

	ui.dateEdit->set_Date(s_date);

	connect(ui.button, &QPushButton::clicked, this, [&] {
		presenter.buttonPressed(ui.dateEdit->getDate());
	});

	connect(ui.dateEdit, &QDateEdit::dateChanged, this, [&] {
		presenter.resetQueue();
		ui.tableWidget->setRowCount(0);
		ui.progressBar->setMaximum(1);
		ui.progressBar->setValue(0);
		ui.progressBar->setTextVisible(false);
		s_date = ui.dateEdit->getDate();
	});


	connect(ui.tableWidget, &QTableWidget::doubleClicked, this,
		[&](const QModelIndex& index) {
			presenter.newAmbList(ui.tableWidget->item(index.row(), 0)->data(0).toLongLong());
	});

	ui.tableWidget->hideColumn(0);
	ui.tableWidget->setColumnWidth(1, 180);
	ui.tableWidget->setColumnWidth(3, 150);
	ui.tableWidget->setColumnWidth(4, 80);

	if (s_cache.empty()) return;

	//fetch table data from the static cache
	const int cCount = 8;
	int rCount = s_cache.size() / 8;

	for (int r = 0; r < rCount; r++) {

		ui.tableWidget->insertRow(r);

		for (int c = 0; c < cCount; c++)
		{
			auto item = new QTableWidgetItem(s_cache[r * cCount + c]);
			
			if (c > 1) item->setTextAlignment(Qt::AlignCenter);

			ui.tableWidget->setItem(r, c, item);
		}
	}
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
{
	//caching the table data
	if (ui.tableWidget->rowCount()) {

		auto cCount = ui.tableWidget->columnCount();
		auto rCount = ui.tableWidget->rowCount();

		s_cache.reserve(cCount * rCount);

		for (int r = 0; r < rCount; r++)
			for (int c = 0; c < cCount; c++)
				s_cache.push_back(ui.tableWidget->item(r, c)->data(0).toString());
	}

}
