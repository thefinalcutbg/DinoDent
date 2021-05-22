#include "ToothGraphicsItem.h"


ToothGraphicsItem::ToothGraphicsItem(int index) : index(index), toothGraphic(nullptr)
{


    bounds.setHeight(224);

    if (index > 2 && index < 13 || index > 18 && index < 29)
        bounds.setWidth(36);
    else
        bounds.setWidth(54);
}

ToothGraphicsItem::~ToothGraphicsItem()
{
}



QRectF ToothGraphicsItem::boundingRect() const
{
    return bounds;
}

void ToothGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //painter->fillRect(bounds.toRect(), Qt::white);

    if (toothGraphic != NULL)
    {
        painter->drawPixmap(bounds.toRect(), *toothGraphic);
    }

    //painter->setPen(Qt::black);
    //painter->drawRect(bounds.toRect());

}

void ToothGraphicsItem::setToothGraphic(QPixmap* toothGraphic)
{
    if (this->toothGraphic != NULL) delete this->toothGraphic;
    this->toothGraphic = toothGraphic;
    update();
}
