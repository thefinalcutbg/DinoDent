#pragma once

#include <QWidget>
#include "ui_PatientSummary.h"
#include "IPatientSummaryView.h"
#include "View/ListView/TeethView/TeethViewScene.h"
#include "View/ProcedureDisplayModel/ProcedureTableModel.h"
#include "TeethView/TeethBuccalScene.h"
#include "TeethView/TeethLingualScene.h"

class PatientSummary final : public QWidget, public IPatientSummaryView
{
	Q_OBJECT

private:


	PatientSummaryPresenter* presenter;

	void paintEvent(QPaintEvent* event);
	ProcedureTableModel m_procedureModel;
	
	TeethBuccalScene* buccalScene;
	TeethLingualScene* lingualScene;
	QGraphicsScene* emptyScene;
	
public:

	PatientSummary(QWidget* parent = Q_NULLPTR);
	~PatientSummary();

	// Inherited via IPatientSummaryView
	
	void setPresenter(PatientSummaryPresenter* presenter) override;
	void setTickPosition(int idx) override;
	void setTimeFrameCount(int count) override;
	void setPatient(const Patient& patient) override;
	void setTeeth(const std::array<ToothPaintHint, 32>& teeth) override;
	void setProcedures(const std::vector<Procedure>& p) override;
	void setPerioData(const PerioWithDisabled& perio) override;
	void setInitialAmbList() override;
	void setPerioStatistic(const PerioStatistic& stat) override;
	void setDocumentLabel(const std::string& label, const std::string& date, const std::string& doctor) override;
	void setToothInfo(const ToothInfoStr& info) override;
	void setSelectedTooth(int toothIdx) override;
	void setPrice(const PriceInfoStr& priceLabel) override;;

private:
	Ui::PatientSummary ui;
};
