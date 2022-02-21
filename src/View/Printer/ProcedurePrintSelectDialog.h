#pragma once

#include <QDialog>
#include "ui_ProcedurePrintSelectDialog.h"
#include <vector>
#include <optional>
#include "Model/Procedure/Procedure.h"

#include "View/ProcedureDisplayModel/ProcedureTableModel.h"

class ProcedurePrintSelectDialog : public QDialog
{
	Q_OBJECT

	std::vector<int> m_selectedRows;
	ProcedureTableModel model;

public:
	ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, QWidget *parent = Q_NULLPTR);
	const std::vector<int> selectedProcedures() const;
	~ProcedurePrintSelectDialog();


private:
	Ui::ProcedurePrintSelectDialog ui;
};
