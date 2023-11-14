#pragma once
#include <QWidget>

#include "ui_SurfacePanel.h"


#include "View/Graphics/CPTooth.h"
#include "View/Graphics/ControlPanelPolygon.h"

#include "View/Interfaces/ISurfacePanel.h"
#include "Presenter/InputEnums.h"


class SurfacePanelPresenter;

class SurfacePanel : public QWidget, public PolygonObserver, public ISurfacePanel
{
	Q_OBJECT

	QGraphicsScene* occlusal_scene;
	QGraphicsScene* buccal_scene;

	SurfacePanelPresenter* presenter;

	CPTooth* occlusal_toothGraphic;
	CPTooth* buccal_toothGraphic;

	std::array<ControlPanelPolygon*, 5> occlusal_polygon;
	std::array<ControlPanelPolygon*, 5> buccal_polygon;

	std::array<QString, 10> labels;

public:
	SurfacePanel(QWidget *parent = Q_NULLPTR);
	~SurfacePanel();


	// Inherited via ISurface Panel
	void setPresenter(SurfacePanelPresenter* presenter) override;
	void paintTooth(const ToothPaintHint& tooth) override;
	void hidePanel(bool hidden) override;
	void setLabels(std::array<std::string, 10> SurfaceNames);
	void setSideButtonsClicked(bool obturation, bool caries);
	// Inherited via PolygonObserver
	virtual void buttonHovered(ButtonPos position, Hover hoverState);
	virtual void buttonClicked(ButtonPos position, MouseClick click);

private:
	Ui::SurfacePanel ui;


};
