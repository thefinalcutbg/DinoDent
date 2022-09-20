#pragma once
#include <QGraphicsItem>
#include "Presenter/InputEnums.h"

class PerioView;

enum class PerioGraphicsType{Plaque, Bleeding};

constexpr double surfButtonWidth = 70.25;
constexpr double surfButtonHeight = 30;

class PerioGraphicsButton : public QGraphicsItem
{
	QPolygonF surface;
	QPainterPath path;

	PerioGraphicsType m_type;

	inline static constexpr QRectF bounds{ 0,0, surfButtonWidth, surfButtonHeight };
	
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

