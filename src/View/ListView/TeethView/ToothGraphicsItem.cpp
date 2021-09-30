#include "ToothGraphicsItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

ToothGraphicsItem::ToothGraphicsItem(int index) : index(index), hasProcedure(false), toothGraphic(nullptr)
{

    bounds.setHeight(224);

    if (index > 2 && index < 13 || index > 18 && index < 29)
        bounds.setWidth(36);
    else
        bounds.setWidth(54);

    index < 16 ?
        procedureMarkerHeight = 0
        :
        procedureMarkerHeight = 209;

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

    if (hasProcedure)
    {
        painter->setOpacity(0.2);
        painter->fillRect(0, procedureMarkerHeight, bounds.width(), 15, QBrush(Qt::green));
        painter->setOpacity(1);
    }

    if (toothGraphic != NULL)
    {
        painter->drawPixmap(bounds.toRect(), *toothGraphic);
    }
    /*
    QColor color(Qt::GlobalColor::black);
    painter->setPen(color);
    painter->drawRect(bounds);
    */
}

void ToothGraphicsItem::showLingual(bool show)
{
    bounds.setHeight(show ? 332 : 224);

    update();
}

void ToothGraphicsItem::setProcedure(bool hasProcedure)
{
    this->hasProcedure = hasProcedure;
    update();
}

void ToothGraphicsItem::setToothGraphic(QPixmap* toothGraphic)
{
    if (this->toothGraphic != NULL) delete this->toothGraphic;
    this->toothGraphic = toothGraphic;
    update();
}

void ToothGraphicsItem::setToothPixmap(const QPixmap& px)
{
    m_tooth = px;
    update();
}
