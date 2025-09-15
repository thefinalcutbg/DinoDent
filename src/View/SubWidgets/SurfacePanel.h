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
	PolygonBorder* polygon_border;
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
	void paintTooth(const ToothPaintHint& tooth, bool hasNotes) override;
	void hidePanel(bool hidden) override;
    void setLabels(std::array<std::string, 6>& SurfaceNames) override;
    void setStatuses(std::array<std::string, 6>& StatusNames) override;
	// Inherited via PolygonObserver
    virtual void buttonHovered(ButtonPos position, Hover hoverState) override;
    virtual void buttonClicked(ButtonPos position, MouseClick click) override;

private:
	Ui::SurfacePanel ui;


};
