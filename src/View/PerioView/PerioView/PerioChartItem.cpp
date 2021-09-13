#include "PerioChartItem.h"
#include <QPainter>
#include "../widgets/Measurment.h"

constexpr int y_coef = 50;

PerioChartItem::PerioChartItem()
{

	double xPos = 7.8;

	for (int i = 0; i < 48; i++)
	{
		gm_polygon << QPointF(xPos, y_coef);
		cal_polygon << QPointF(xPos, y_coef);
		xPos = xPos+ 21.35;
	}
	
}

QRectF PerioChartItem::boundingRect() const
{
	return bounds;
}

void PerioChartItem::setMeasurment(const Measurment& m)
{
	gm_polygon[m.position].setY((m.GM*3+ y_coef));
	cal_polygon[m.position].setY(m.CAL * (-3) + y_coef);
	update();
}



void PerioChartItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

	//painter->fillRect(bounds, Qt::GlobalColor::lightGray);

	QPen pen; pen.setColor(QColor(Qt::GlobalColor::red)); pen.setWidth(1);
	painter->setPen(pen);

	QPainterPath path1; path1.addPolygon(cal_polygon);
	painter->drawPath(path1);

	pen.setWidth(2);

	pen.setColor(Qt::GlobalColor::lightGray);
	painter->setPen(pen);

	painter->drawLine(QPointF(0, y_coef), QPointF(bounds.width(), y_coef));

	pen.setColor(Qt::GlobalColor::gray);
	painter->setPen(pen);


	QPainterPath path; path.addPolygon(gm_polygon);
	painter->drawPath(path);

	

}

PerioChartItem::~PerioChartItem()
{
}
