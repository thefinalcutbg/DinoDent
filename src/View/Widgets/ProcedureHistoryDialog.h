#pragma once

#include <QDialog>
#include "ui_ProcedureHistoryDialog.h"

#include "View/Graphics/TeethViewScene.h"
#include "View/TableModels/ProcedureTableModel.h"
#include "View/TableModels/HospitalizationTableModel.h"
#include "Model/Dental/Procedure.h"
#include "Presenter/ProcedureHistoryPresenter.h"

class ProcedureHistoryDialog : public QDialog
{
	Q_OBJECT

	ProcedureTableModel pis_model, his_model;
	HospitalizationTableModel hospi_model;
	ProcedureHistoryPresenter& presenter;

public:
	bool applyProcedures{ false };
	ProcedureHistoryDialog(ProcedureHistoryPresenter& p);

	// Inherited via IProcedureHistoryDialog
	void setPis(const std::vector<Procedure>& p);
	void setHis(const std::vector<Procedure>& h);
	void setHospitalizations(const std::vector<Hospitalization>& h);
	void setSnapshots(const std::vector<HisSnapshot>& snapshots);
	SnapshotViewer* getSnapshotViewer();
	void focusTab(int idx);
	void closeDialog();

	~ProcedureHistoryDialog();

private:
	Ui::ProcedureHistoryDialog ui;


};
