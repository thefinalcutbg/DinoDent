#pragma once

#include <QWidget>
#include "ui_SurfacePanel.h"


#include "../ToothPaintDevices/CPToothPainter.h"

#include "CPTooth.h"
#include "ControlPanelPolygon.h"

#include "View/ListView/SurfacePanel/ISurfacePanel.h"
#include "Presenter/SurfacePanel/SurfacePanelPresenter.h"
#include "Presenter/SurfacePanel/InputEnums.h"

class SurfacePanel : public QWidget, public PolygonObserver, public ISurfacePanel
{
	Q_OBJECT

	QGraphicsScene* scene;

	SurfacePanelPresenter presenter;

	CPToothPainter painter;

	CPTooth* toothGraphic;
	std::array<ControlPanelPolygon*, 5> polygon;

	std::array<QString, 6> labels;
	std::array<QString, 6> statuses;

public:
	SurfacePanel(QWidget *parent = Q_NULLPTR);
	~SurfacePanel();

	SurfacePanelPresenter* getPresenter();

	// Inherited via ISurface Panel
	void paintTooth(const Tooth* tooth);
	void showPanel(bool show);
	void setLabels(std::array<std::string, 6> SurfaceNames);
	void setStatuses(std::array<std::string, 6> StatusNames);
	void setSideButtonsClicked(bool obturation, bool caries);
	// Inherited via PolygonObserver
	virtual void buttonHovered(ButtonPos position, Hover hoverState);
	virtual void buttonClicked(ButtonPos position, MouseClick click);

private:
	Ui::SurfacePanel ui;


};
