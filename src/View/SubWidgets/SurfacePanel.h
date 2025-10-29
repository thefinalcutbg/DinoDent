#pragma once
#include <QWidget>

#include <array>
#include <string>

#include "ui_SurfacePanel.h"

#include "View/Graphics/CPTooth.h"
#include "View/Graphics/ControlPanelPolygon.h"

#include "Presenter/InputEnums.h"

struct ToothPaintHint;
class SurfacePanelPresenter;

class SurfacePanelPresenter;

class SurfacePanel : public QWidget, public PolygonObserver
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
	void setPresenter(SurfacePanelPresenter* presenter);
	void paintTooth(const ToothPaintHint& tooth, bool hasNotes);
	void hidePanel(bool hidden);
    void setLabels(std::array<std::string, 6>& SurfaceNames);
    void setStatuses(std::array<std::string, 6>& StatusNames);
	~SurfacePanel();

	// Inherited via PolygonObserver
    virtual void buttonHovered(ButtonPos position, Hover hoverState);
    virtual void buttonClicked(ButtonPos position, MouseClick click);

private:
	Ui::SurfacePanel ui;


};
