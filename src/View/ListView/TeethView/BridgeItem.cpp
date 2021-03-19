#include "BridgeItem.h"

BridgeItem::BridgeItem() : bounds{ 0, 0, 684, 45 }, paintedBridge(NULL)
{
}

void BridgeItem::setTexture(QPixmap* pixmap)
{
	if (paintedBridge != NULL) {
		delete paintedBridge;
	}

	paintedBridge = pixmap;
	update();
}

QRectF BridgeItem::boundingRect() const
{
	return QRectF(0, 0, 684, 45);
}

void BridgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	if (paintedBridge != NULL)
	{
		painter->drawPixmap(bounds.toRect(), *paintedBridge);
	}
	

	//painter->setOpacity(0.5);
	//painter->fillRect(bounds, Qt::gray);
	
}
