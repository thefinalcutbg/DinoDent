#pragma once
#include <QWidget>

#include "ui_SurfacePanel.h"


#include "CPTooth.h"
#include "ControlPanelPolygon.h"

#include "View/ListView/SurfacePanel/ISurfacePanel.h"
#include "Presenter/ListPresenter/InputEnums.h"


class SurfacePanelPresenter;

class SurfacePanel : public QWidget, public PolygonObserver, public ISurfacePanel
{
	Q_OBJECT

	QGraphicsScene* scene;

	SurfacePanelPresenter* presenter;

	CPTooth* toothGraphic;
	std::array<ControlPanelPolygon*, 5> polygon;

	std::array<QString, 6> labels;
	std::array<QString, 6> statuses;

public:
	SurfacePanel(QWidget *parent = Q_NULLPTR);
	~SurfacePanel();


	// Inherited via ISurface Panel
	void setPresenter(SurfacePanelPresenter* presenter) override;
	void paintTooth(const ToothPaintHint& tooth) override;
	void hidePanel(bool hidden) override;
	void setLabels(std::array<std::string, 6> SurfaceNames);
	void setStatuses(std::array<std::string, 6> StatusNames);
	void setSideButtonsClicked(bool obturation, bool caries);
	// Inherited via PolygonObserver
	virtual void buttonHovered(ButtonPos position, Hover hoverState);
	virtual void buttonClicked(ButtonPos position, MouseClick click);

private:
	Ui::SurfacePanel ui;


};
