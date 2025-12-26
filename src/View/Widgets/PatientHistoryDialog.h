#pragma once

#include <QDialog>
#include "ui_PatientHistoryDialog.h"
#include "Model/Dental/HisSnapshot.h"
#include "Model/PlainTable.h"
#include "Model/Dental/Procedure.h"
#include "Model/Dental/PerioStatistic.h"
#include "View/TableModels/PlainTableModel.h"
#include "View/TableModels/ProcedureTableModel.h"
#include "View/TableModels/HospitalizationTableModel.h"

class PatientHistoryPresenter;


class PatientHistoryDialog : public QDialog
{
	Q_OBJECT

	static constexpr int PERIO_TAB_INDEX = 4;

	PatientHistoryPresenter& presenter;

	PlainTableModel doc_model;
	PlainTableModel doc_details_model;
	std::vector<PlainTable> details_data;
	
	HospitalizationTableModel hospi_model;
	ProcedureTableModel procedure_model;

	void tabChanged(int idx);
	Procedure::DatabaseSource getProcedureSrc();

public:
	PatientHistoryDialog(PatientHistoryPresenter& presenter, QWidget *parent = nullptr);
	void setProcedures(const std::vector<Procedure> procedures, Procedure::DatabaseSource source);
	void setDocuments(const PlainTable& docList, const std::vector<PlainTable>& contents);
	void setSnapshots(const std::vector<HisSnapshot>& snapshots, Procedure::DatabaseSource source);
	void setPerioSnapshots(const std::vector<PerioSnapshot>& snapshots);
	void setHospitalizations(const std::vector<Hospitalization>& h);
	void setPatientNoteFlags (const std::array<std::string, 32>& notes);
	void hideNhif(bool hidden);
    void open(Procedure::DatabaseSource src, bool showApplyToStatus);
	~PatientHistoryDialog();

private:
	Ui::PatientHistoryDialogClass ui;
};
