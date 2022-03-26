#pragma once

#include <QDialog>
#include "ui_ProcedurePrintSelectDialog.h"
#include <vector>
#include <optional>
#include "Model/Procedure/Procedure.h"

#include "View/ProcedureDisplayModel/ProcedureSelectModel.h"

class ProcedurePrintSelectDialog : public QDialog
{
	Q_OBJECT

	std::vector<int> m_selectedRows;
	ProcedureSelectModel model;

public:
	ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, QWidget *parent = Q_NULLPTR);
	const std::vector<int> selectedProcedures() const;
	void selectOnlyWhereNzokIs(bool nzok);

	~ProcedurePrintSelectDialog();


private:
	Ui::ProcedurePrintSelectDialog ui;
};
