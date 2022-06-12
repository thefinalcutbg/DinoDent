#include "PISHistoryDialog.h"

PISHistoryDialog::PISHistoryDialog(const std::vector<Procedure> procedures, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(u8"Отчетени манипулации по НЗОК");

	model.setProcedures(procedures);


	ui.tableView->setModel(&this->model);
	
    //ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    ui.tableView->hideColumn(0);
    ui.tableView->setColumnWidth(1, 69);
    ui.tableView->setColumnWidth(2, 200);
    ui.tableView->setColumnWidth(3, 25);
    ui.tableView->setColumnWidth(4, 300);
    ui.tableView->setColumnWidth(5, 49);
    ui.tableView->hideColumn(6);
    ui.tableView->hideColumn(7);
    ui.tableView->verticalHeader()->setDefaultSectionSize(20);
    ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui.tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    //setShowGrid(false);

    connect(ui.applyButton, &QPushButton::clicked, this, [&]{applyProcedures = true; close();});

}

PISHistoryDialog::~PISHistoryDialog()
{
}
