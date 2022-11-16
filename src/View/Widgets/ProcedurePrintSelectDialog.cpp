#include "ProcedurePrintSelectDialog.h"
#include <set>

ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, QWidget* parent)
	: QDialog(parent), model(procedures)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle("Избор на манипулации");

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	ui.tableView->setModel(&this->model);
	ui.tableView->setProcedurePrintSelectLayout();

	connect(ui.okButton, &QPushButton::pressed, [=]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			m_selectedRows = model.selectedRows();

			accept();

		});

	connect(ui.paidButton, &QPushButton::clicked, [=] { this->model.selectOnlyRowsWhereNzokIs(false); });
	connect(ui.nzokButton, &QPushButton::clicked, [=] { this->model.selectOnlyRowsWhereNzokIs(true); });
}

const std::vector<int> ProcedurePrintSelectDialog::selectedProcedures() const
{
	return m_selectedRows;
}

void ProcedurePrintSelectDialog::selectOnlyWhereNzokIs(bool nhif)
{
	model.selectOnlyRowsWhereNzokIs(nhif);
}


ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
