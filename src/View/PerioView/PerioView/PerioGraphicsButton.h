#pragma once
#include <QGraphicsItem>
#include "Presenter/ListPresenter/InputEnums.h"

class PerioView;

constexpr int surfButtonWidth = 70;

class PerioGraphicsButton : public QGraphicsItem
{
	QPolygonF surface;
	QPainterPath path;

	QRectF bounds{ 0,0, 70, 30 };
	
	bool m_checked{ false };
	bool m_hovered{ false };

	bool m_disabled{ false };

	QColor m_checkedColor{ 204, 228, 247 };
	PerioView* view;

	QPainterPath shape() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
	PerioGraphicsButton(ButtonPos position, PerioView* perioView);
	void setColor(QColor color);
	void setDisabled(bool disabled);
	bool isEnabled();
	QRectF boundingRect() const override;

};

