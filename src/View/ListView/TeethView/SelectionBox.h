#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>


class SelectionBox : public QGraphicsItem
{
	int index;

public:
	SelectionBox(int index);
	~SelectionBox();
	
protected:

	bool hovered;
	QRectF bounds;
	QRectF boundingRect() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;


public:
	int getIndex();

};
