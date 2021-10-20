#pragma once
#include <QGraphicsItem>
#include "Presenter/ListPresenter/InputEnums.h"

class PerioView;

enum class PerioGraphicsType{Plaque, Bleeding};

constexpr int surfButtonWidth = 70;

class PerioGraphicsButton : public QGraphicsItem
{
	QPolygonF surface;
	QPainterPath path;

	PerioGraphicsType m_type;

	QRectF bounds{ 0,0, 70, 30 };
	
	int m_index;

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
	PerioGraphicsButton(int index, PerioGraphicsType type, PerioView* perioView);

	void setDisabled(bool disabled);
	bool isEnabled();
	bool isChecked();
	void setChecked(bool checked);
	QRectF boundingRect() const override;

};

