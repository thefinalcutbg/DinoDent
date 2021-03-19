#pragma once

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>

class BridgeItem : public QGraphicsItem
{
	QPixmap* paintedBridge;
	QRectF bounds;
public:
	BridgeItem();

	void setTexture(QPixmap* pixmap);

	QRectF boundingRect() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

