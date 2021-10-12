#include "ToothGraphicsItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

ToothGraphicsItem::ToothGraphicsItem(int index) : index(index), hasProcedure(false)
{

    bounds.setHeight(224);
    pxHeight = 224;

    if (index > 2 && index < 13 || index > 18 && index < 29)
    {
        bounds.setWidth(36);
        pxWidth = 36;
    }

    else
    {
        bounds.setWidth(54);
        pxWidth = 54;
    }
        

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

    int xPos = (bounds.width() / 2) - (pxWidth / 2);

    painter->drawPixmap(xPos, 0, pxWidth, pxHeight, m_tooth);

    if (bounds.width() > pxWidth)
    {
        painter->drawPixmap(QRect(0, 0, xPos+1, pxHeight), m_tooth, QRect(1, 0, 1, m_tooth.height()));
        
        painter->drawPixmap(QRect(xPos+pxWidth-1, 0, xPos+1, pxHeight), m_tooth, QRect(m_tooth.width() - 2, 0, 1, m_tooth.height()));

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
    pxHeight = show ? 332 : 224;
    update();
}

void ToothGraphicsItem::setProcedure(bool hasProcedure)
{
    this->hasProcedure = hasProcedure;
    update();
}

void ToothGraphicsItem::setWidth(int width)
{
    if (width < pxWidth) return;

    bounds.setWidth(width);
    update();
}


void ToothGraphicsItem::setToothPixmap(const QPixmap& px)
{
    m_tooth = px;
    update();
}
