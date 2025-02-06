#pragma once

#include <QWidget>
#include "ui_PatientSummary.h"
#include "View/Interfaces/IPatientSummaryView.h"
#include "View/TableModels/ProcedureTableModel.h"
#include "View/Graphics/TeethBuccalScene.h"
#include "View/Graphics/TeethLingualScene.h"

struct PatientSummary final : public QWidget, public IPatientSummaryView
{
	Q_OBJECT

private:


	PatientSummaryPresenter* presenter;

    void paintEvent(QPaintEvent* event) override;
	ProcedureTableModel m_procedureModel;
	
	TeethBuccalScene* buccalScene;
	TeethLingualScene* lingualScene;
	QGraphicsScene* emptyScene;
	
public:

	PatientSummary(QWidget* parent = Q_NULLPTR);
	~PatientSummary();

	// Inherited via IPatientSummaryView
	
	void setPresenter(PatientSummaryPresenter* presenter) override;
	IPatientTileInfo* patientTile() override;
	void setTeeth(const std::array<ToothPaintHint, 32>& teeth) override;
	void setProcedures(const std::vector<Procedure>& p) override;
	void setPerioData(const PerioWithDisabled& perio) override;
	void setInitialAmbList() override;
	void setPerioStatistic(const PerioStatistic& stat) override;
	void setDocumentLabel(const std::string& label, const std::string& date, const std::string& doctor) override;
	void setToothInfo(const ToothInfoStr& info) override;
	void hideToothInfo() override;
	void setUiState(const SummaryState& state) override;
private:
	Ui::PatientSummary ui;
};
