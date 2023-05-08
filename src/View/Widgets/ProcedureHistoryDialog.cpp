#include "ProcedureHistoryDialog.h"

ProcedureHistoryDialog::ProcedureHistoryDialog(const std::vector<Procedure> procedures, const std::string& title, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(title.c_str());

	model.setProcedures(procedures);


	ui.tableView->setModel(&this->model);
	
    ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

    ui.tableView->hideColumn(0);
    ui.tableView->hideColumn(6);
    ui.tableView->hideColumn(7);

    ui.tableView->setColumnWidth(1, 69);
    ui.tableView->setColumnWidth(2, 200);
    ui.tableView->setColumnWidth(3, 25);
    ui.tableView->setColumnWidth(4, 200);
    ui.tableView->setColumnWidth(5, 49);
    ui.tableView->setColumnWidth(8, 200);

    ui.tableView->verticalHeader()->setDefaultSectionSize(20);
    ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

    connect(ui.applyButton, &QPushButton::clicked, this, [&]{applyProcedures = true; close();});

}

ProcedureHistoryDialog::~ProcedureHistoryDialog()
{
}
