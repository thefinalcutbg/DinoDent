#pragma once

#include <QDialog>
#include "ui_PISHistoryDialog.h"
#include "View/Models/ProcedureTableModel.h"
#include "Model/Dental/Procedure.h"

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
