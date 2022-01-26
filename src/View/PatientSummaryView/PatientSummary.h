#pragma once

#include <QWidget>
#include "ui_PatientSummary.h"
#include "IPatientSummaryView.h"

class PatientSummary final : public QWidget, public IPatientSummaryView
{
	Q_OBJECT

private:


	PatientSummaryPresenter* presenter;

	void paintEvent(QPaintEvent* event);

	

public:

	PatientSummary(QWidget* parent = Q_NULLPTR);
	~PatientSummary();



	// Inherited via IPatientSummaryView
	void setPresenter(PatientSummaryPresenter* presenter) final;
	void setPatient(const Patient& patient) final;
	void setAmbDates(const std::vector<Date>& ambDates) final;
	void setTeeth(const ToothPaintHint& tooth) final;
	void setProcedures(const std::vector<Procedure>& m) final;

private:
	Ui::PatientSummary ui;
};
