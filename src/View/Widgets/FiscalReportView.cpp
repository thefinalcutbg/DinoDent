#include "FiscalReportView.h"
#include "QtVersion.h"
#include "Model/Date.h"
#include <QPainter>

FiscalReportView::FiscalReportView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	auto d = Date::currentDate();
	
	ui.monthCombo->setCurrentIndex(d.month - 1);
	ui.yearSpin->setValue(d.year);

	ui.tableView->setModel(&data_model);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	ui.tableView->horizontalHeader()->setHighlightSections(false);

	ui.tableView->verticalHeader()->hide();

	ui.tableView->setColumnWidth(0, 150);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 100);
	ui.tableView->setColumnWidth(3, 200);

	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

	connect(ui.monthCombo, QtComboIndexChanged, [&](int index) {presenter.dateChanged(index + 1, ui.yearSpin->value()); });
	connect(ui.yearSpin, QtSpinValueChanged, [&](int value) { presenter.dateChanged(ui.monthCombo->currentIndex() + 1, value); });

	connect(ui.deleteReceipt, &QPushButton::clicked, [&] { presenter.deleteReceipt(getCurrentIndex()); });
	connect(ui.generateFiscalDescr, &QPushButton::clicked, [&] { presenter.generateDescription(); });
	connect(ui.generateInvoice, &QPushButton::clicked, [&] { presenter.generateInvoice(); });
	connect(ui.tableView, &QTableView::doubleClicked, [&] { presenter.editReceipt(getCurrentIndex()); });


	presenter.dateChanged(d.month, d.year);

	presenter.setView(this);



}

int FiscalReportView::getCurrentIndex()
{
	return ui.tableView->selectionModel()->currentIndex().row();
}

	void FiscalReportView::setFiscalData(const std::vector<FiscalReceipt>&data)
{
	data_model.setRows(data);
}


FiscalReportView::~FiscalReportView()
{}

void FiscalReportView::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}
