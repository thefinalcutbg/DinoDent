#pragma once
#include <QGraphicsItem>
#include <array>

class HexagonGraphicsItem : public QGraphicsItem
{

	QRectF bounds{ 0,0,300,300 };

	QPolygonF riskPoly;
	QPolygonF polygon[6];

	QColor riskColor;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;


public:
	HexagonGraphicsItem();

	QRectF boundingRect() const override { return bounds; };

	void setRiskValue(const std::array<int, 6>& values, int risk);

	~HexagonGraphicsItem();
};
