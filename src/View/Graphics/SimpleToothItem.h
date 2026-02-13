#pragma once

#include "SelectableGraphicsItem.h"
#include <string>
#include <set>

class ToothPaintHint;

class SimpleToothItem  : public SelectableGraphicsItem
{
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
	void hoverEnterEvent(QGraphicsSceneHoverEvent*) override { hovered = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override { hovered = false; update(); }
	QString m_num;
	bool m_hasProcedure = false;
	QStringList m_stat_list;

public:
	SimpleToothItem(int idx);
	void setData(const std::string& num, const std::set<std::string>& statuses);
	void resetHoverState() { hovered = false; update(); }
	void setProcedure(bool hasProcedure) { m_hasProcedure = hasProcedure; update(); }
	~SimpleToothItem();

	QRectF boundingRect() const override;
};

