#pragma once

#include <QGraphicsItem>


class PerioChartItem : public QGraphicsItem
{

	QPolygonF gm_polygon;
	QPolygonF cal_polygon;

	QRectF bounds;

	bool m_upsideDown{ false };

public:
	PerioChartItem(bool even = true);

	QRectF boundingRect() const override;
	void setMeasurment(int idx, int gm, int cal, bool repaint = true);
	
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	~PerioChartItem();
};
