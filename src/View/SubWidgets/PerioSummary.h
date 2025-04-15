#pragma once

#include <QWidget>
#include "ui_PerioSummary.h"

#include "View/Graphics/TeethBuccalScene.h"
#include "View/Graphics/TeethLingualScene.h"
#include "Model/Dental/PerioStatus.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/PerioStatistic.h"

struct PerioSnapshot {
	PerioStatus perioStatus;
	ToothContainer toothStatus;
	PerioStatistic perioStatistic;
};


class PerioSummary : public QWidget
{
	Q_OBJECT

	TeethBuccalScene* buccalScene;
	TeethLingualScene* lingualScene;

	void paintEvent(QPaintEvent* e) override;

	void displaySnapshotToView(int index);

	std::vector<PerioSnapshot> m_snapshots;

public:
	PerioSummary(QWidget *parent = nullptr);
	void setSnapshots(const std::vector<PerioSnapshot>& snapshots);
	~PerioSummary() {};

private:
	Ui::PerioSummaryClass ui;
};
