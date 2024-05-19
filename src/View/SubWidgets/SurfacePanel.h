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

	QGraphicsScene* scene;

	SurfacePanelPresenter* presenter;

	CPTooth* toothGraphic;
	std::array<ControlPanelPolygon*, 5> polygon;

	std::array<QString, 6> labels;
	std::array<QString, 6> statuses;

	bool m_focused = false;

	void paintEvent(QPaintEvent* e) override;

public:
	SurfacePanel(QWidget *parent = Q_NULLPTR);
	void drawFocused(bool focused);
	~SurfacePanel();


	// Inherited via ISurface Panel
	void setPresenter(SurfacePanelPresenter* presenter) override;
	void paintTooth(const ToothPaintHint& tooth) override;
	void hidePanel(bool hidden) override;
	void setLabels(std::array<std::string, 6> SurfaceNames);
	void setStatuses(std::array<std::string, 6> StatusNames);
	void setSideButtonsClicked(bool restoration, bool caries);
	// Inherited via PolygonObserver
	virtual void buttonHovered(ButtonPos position, Hover hoverState);
	virtual void buttonClicked(ButtonPos position, MouseClick click);

private:
	Ui::SurfacePanel ui;


};
