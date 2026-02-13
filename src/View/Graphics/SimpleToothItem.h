#pragma once

#include "SelectableGraphicsItem.h"
#include <string>
#include <set>

class ToothPaintHint;

class SimpleToothItem  : public SelectableGraphicsItem
{
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	QString m_num;
	QStringList m_stat_list;

public:
	SimpleToothItem(int idx);
	void setData(const std::string& num, const std::set<std::string>& statuses);
	~SimpleToothItem();

	QRectF boundingRect() const override;
};

