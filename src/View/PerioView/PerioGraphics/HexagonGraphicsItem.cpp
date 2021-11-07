#include "HexagonGraphicsItem.h"
#include <QPainter>

QPolygon getHexaPolygon(int radius)
{
	QPolygon poly;

	poly
		<< QPoint(0, radius * (-1))
		<< QPoint(radius, (radius / 2) * (-1))
		<< QPoint(radius, radius / 2)
		<< QPoint(0, radius)
		<< QPoint(radius * (-1), radius / 2)
		<< QPoint(radius * (-1), (radius / 2) * (-1));

	return poly;
}

HexagonGraphicsItem::HexagonGraphicsItem()
{

	int radius{ 20 };

	for (int i = 0; i < 6; i++)
	{
		polygon[i] = getHexaPolygon(radius);
		polygon[i].translate(bounds.center().toPoint());
		radius += 20;
	}

	riskPoly = polygon[0];


}


void HexagonGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

	QPainterPath riskPath;
	riskPath.addPolygon(riskPoly);
	painter->setOpacity(0.5);
	painter->fillPath(riskPath, riskColor);

	painter->setOpacity(1);

	for (int i = 0; i < 6; i++)
	{
		QPen pen;
		pen.setWidth(i % 2 ? 2 : 1);
		painter->setPen(pen);
		painter->drawPolygon(polygon[i]);
	}

	QPen pen; pen.setWidth(1);

	painter->setPen(pen);

	for (int i = 0; i < 6; i++)
	{
		painter->drawLine(bounds.center(), polygon[5][i]);
	}



}

void HexagonGraphicsItem::setRiskValue(const std::array<int, 6>& values, int risk)
{

	riskPoly.clear();
	riskPoly.reserve(6);

	for (int i = 0; i < values.size(); i++)
	{
		riskPoly << polygon[values[i]][i];
	}

	switch (risk)
	{
		case 0: riskColor = QColor(Qt::GlobalColor::green); break;
		case 1: riskColor = QColor(247, 167, 62); break;
		case 2: riskColor = QColor(Qt::GlobalColor::red); break;
		default: riskColor = QColor(Qt::GlobalColor::white); break;
	}

	update();
}

HexagonGraphicsItem::~HexagonGraphicsItem()
{
}
