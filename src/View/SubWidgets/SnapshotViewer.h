#pragma once

#include <QWidget>
#include "ui_SnapshotViewer.h"
#include "View/Graphics/TeethViewScene.h"
#include "Model/Dental/HisSnapshot.h"

class SnapshotViewer : public QWidget
{
	Q_OBJECT

	TeethViewScene* teeth_scene;
	std::vector<HisSnapshot> m_snapshots;

	void paintEvent(QPaintEvent* e) override;

	void displaySnapshotToView(const HisSnapshot& h);

public:
	SnapshotViewer(QWidget *parent = nullptr);
	TeethViewScene* getTeethScene() { return teeth_scene; }
	void setSnapshots(const std::vector<HisSnapshot>& snapshots);
	const HisSnapshot* getCurrentSnapshot() const;
	bool isMostRecent() const;
	~SnapshotViewer();

private:
	Ui::SnapshotViewerClass ui;
};
