#pragma once

#include <QGraphicsItem>

struct Measurment;

class PerioChartItem : public QGraphicsItem
{

	QPolygonF gm_polygon;
	QPolygonF cal_polygon;

	QRectF bounds{ 0, 0, 1020, 100 };

public:
	PerioChartItem();

	QRectF boundingRect() const override;
	void setMeasurment(const Measurment& m);
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	~PerioChartItem();
};
