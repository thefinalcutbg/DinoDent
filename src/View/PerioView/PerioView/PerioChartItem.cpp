#include "PerioChartItem.h"
#include <QPainter>
#include "../widgets/Measurment.h"
#include "Model/Tooth/ToothUtils.h"

constexpr int height = 180;

constexpr int y_pos = 115;
constexpr int y_coef = 3;


double getWidth(int index)
{
	constexpr double width[3]{ 54 , 36 , 36 };

	auto toothType = static_cast<int>(ToothUtils::getToothType(index / 3));

	return width[toothType];
}

double calculateOffset(int index)
{
	auto toothWidth = getWidth(index);

	if(index % 3 == 0 && index)

		return (getWidth(index - 1) / 4) + (toothWidth / 4);

	return toothWidth / 4;

}


PerioChartItem::PerioChartItem(bool even) : bounds{0,0, 1120, height}
{

	bounds.setWidth(even ? 1120 : 684);

	gm_polygon << QPointF(0, y_pos);
	cal_polygon << QPointF(0, y_pos);

	double evenOffset = bounds.width() / 48;

	double xPos = even ? evenOffset / 2 : calculateOffset(0)/2;

	for (int i = 0; i < 48; i++)
	{

		gm_polygon << QPointF(xPos, y_pos);
		cal_polygon << QPointF(xPos, y_pos);

		if (even)
			xPos = xPos + evenOffset;
		else
			xPos = xPos + calculateOffset(i);
	}

	gm_polygon << QPointF(bounds.width(), y_pos);
	cal_polygon << QPointF(bounds.width(), y_pos);
	
}

QRectF PerioChartItem::boundingRect() const
{
	return bounds;
}

void PerioChartItem::setMeasurment(const Measurment& m)
{

	gm_polygon[m.position+1].setY((m.GM* y_coef + y_pos));
	cal_polygon[m.position+1].setY(m.CAL*(-y_coef) + y_pos);
	update();
}



void PerioChartItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

	//painter->fillRect(bounds, Qt::GlobalColor::lightGray);

	QPen pen; pen.setColor(QColor(Qt::GlobalColor::red)); pen.setWidth(3);
	pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
	painter->setPen(pen);

	QPainterPath path1; path1.addPolygon(cal_polygon);
	painter->drawPath(path1);

	pen.setWidth(2);

	pen.setColor(Qt::GlobalColor::lightGray);
	painter->setPen(pen);

	//painter->drawLine(QPointF(0, y_coef), QPointF(bounds.width(), y_coef));

	pen.setColor(Qt::GlobalColor::darkGray);
	painter->setPen(pen);


	QPainterPath path; path.addPolygon(gm_polygon);
	painter->drawPath(path);

	/*
	QColor color(Qt::GlobalColor::green);
	painter->setPen(color);
	painter->drawRect(bounds);
	*/

}

PerioChartItem::~PerioChartItem()
{
}
