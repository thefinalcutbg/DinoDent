#pragma once

#include <QDialog>
#include "ui_ProcedureHistoryDialog.h"
#include "View/TableModels/ProcedureTableModel.h"
#include "Model/Dental/Procedure.h"

class ProcedureHistoryDialog : public QDialog
{
	Q_OBJECT

	ProcedureTableModel model;
	

public:
	bool applyProcedures{ false };
	ProcedureHistoryDialog(const std::vector<Procedure> procedures, const std::string& title, QWidget *parent = Q_NULLPTR);
	~ProcedureHistoryDialog();

private:
	Ui::ProcedureHistoryDialog ui;
};
