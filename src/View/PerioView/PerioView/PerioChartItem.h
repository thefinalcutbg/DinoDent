#pragma once

#include <QGraphicsItem>

struct Measurment;

class PerioChartItem : public QGraphicsItem
{

	QPolygonF gm_polygon;
	QPolygonF cal_polygon;

	QRectF bounds;

	bool m_upsideDown{ false };

public:
	PerioChartItem(bool even = true);

	QRectF boundingRect() const override;
	void setMeasurment(const Measurment& m);
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	~PerioChartItem();
};
