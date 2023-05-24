#pragma once

#include <QDialog>
#include "ui_ProcedureHistoryDialog.h"
#include "View/TableModels/ProcedureTableModel.h"
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/IProcedureHistoryDialog.h"
#include "Presenter/ProcedureHistoryPresenter.h"

class ProcedureHistoryDialog : public QDialog, public IProcedureHistoryDialog
{
	Q_OBJECT

	ProcedureTableModel pis_model, his_model;
	ProcedureHistoryPresenter& presenter;


public:
	bool applyProcedures{ false };
	ProcedureHistoryDialog(ProcedureHistoryPresenter& p);
	~ProcedureHistoryDialog();

private:
	Ui::ProcedureHistoryDialog ui;

	// Inherited via IProcedureHistoryDialog
	void setPis(const std::vector<Procedure>& p) override;
	void setHis(const std::vector<Procedure>& h) override;
	void setCurrentStatus(const ToothContainer& teeth) override;
	void focusHisTab() override;
	void closeDialog() override;
};
