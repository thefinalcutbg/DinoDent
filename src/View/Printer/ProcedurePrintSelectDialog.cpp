#include "ProcedurePrintSelectDialog.h"
#include <set>

ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle(u8"Манипулации за принтиране");

	model.setProcedures(procedures);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	ui.tableView->setModel(&this->model);
	ui.tableView->setProcedureHistoryLayout();

	connect(ui.okButton, &QPushButton::pressed, [=]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			//OPTIMIZE!!!

			std::set<int> selectedRowsSet;

			for (auto& s : select)
			{
				selectedRowsSet.insert(s.row());
			}

			for (auto row : selectedRowsSet)
			{
				m_selectedRows.push_back(row);
			}

			accept();

		});
}

const std::vector<int> ProcedurePrintSelectDialog::selectedProcedures() const
{
	return m_selectedRows;
}


ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
