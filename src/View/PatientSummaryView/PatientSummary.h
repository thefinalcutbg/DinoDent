#pragma once

#include <QWidget>
#include "ui_PatientSummary.h"
#include "IPatientSummaryView.h"
#include "View/ListView/TeethView/TeethViewScene.h"
#include "View/ProcedureDisplayModel/ProcedureTableModel.h"

class PatientSummary final : public QWidget, public IPatientSummaryView
{
	Q_OBJECT

private:


	PatientSummaryPresenter* presenter;

	void paintEvent(QPaintEvent* event);
	ProcedureTableModel m_procedureModel;
	TeethViewScene m_teethScene;

public:

	PatientSummary(QWidget* parent = Q_NULLPTR);
	~PatientSummary();



	// Inherited via IPatientSummaryView
	void setPresenter(PatientSummaryPresenter* presenter) final;
	void setDateLabel(const std::string& dateLabel) final;
	void setTickPosition(int idx) final;
	void setTimeFrameCount(int count) final;
	void setPatient(const Patient& patient) final;
	void setTeeth(const std::array<ToothPaintHint, 32>& teeth) final;
	void setProcedures(const std::vector<Procedure>& p) final;

private:
	Ui::PatientSummary ui;
};