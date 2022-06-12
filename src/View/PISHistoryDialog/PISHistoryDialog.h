#pragma once

#include <QDialog>
#include "ui_PISHistoryDialog.h"
#include "View/ProcedureDisplayModel/ProcedureTableModel.h"
#include "Model/Procedure/Procedure.h"

class PISHistoryDialog : public QDialog
{
	Q_OBJECT

	ProcedureTableModel model;
	

public:
	bool applyProcedures{ false };
	PISHistoryDialog(const std::vector<Procedure> procedures, QWidget *parent = Q_NULLPTR);
	~PISHistoryDialog();

private:
	Ui::PISHistoryDialog ui;
};
