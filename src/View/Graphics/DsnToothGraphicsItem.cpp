#include "DsnToothGraphicsItem.h"
#include <QPainter>
#include <QIcon>
#include <QGraphicsSceneMouseEvent>

DsnToothGraphicsItem::DsnToothGraphicsItem(int index) : index(index), m_tooth(180, 500)
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

}

DsnToothGraphicsItem::~DsnToothGraphicsItem()
{
}

QRectF DsnToothGraphicsItem::boundingRect() const
{
    return bounds;
}

void DsnToothGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    painter->setRenderHint(QPainter::SmoothPixmapTransform);


    int xPos = (bounds.width() / 2) - (pxWidth / 2);

    painter->drawPixmap(xPos, 0, pxWidth, pxHeight, m_tooth);
    
    if (bounds.width() > pxWidth)
    {
        painter->drawPixmap(QRect(0, 0, xPos, pxHeight), m_tooth, QRect(0, 0, 1, m_tooth.height()));
        
        painter->drawPixmap(QRect(xPos+pxWidth, 0, xPos, pxHeight), m_tooth, QRect(m_tooth.width() - 1, 0, 1, m_tooth.height()));
    }
}

void DsnToothGraphicsItem::showLingual(bool show)
{
    bounds.setHeight(show ? 332 : 224);
    pxHeight = show ? 332 : 224;
    update();
}


void DsnToothGraphicsItem::setWidth(int width)
{
    if (width < pxWidth) return;

    bounds.setWidth(width);
    update();
}


void DsnToothGraphicsItem::setToothPixmap(const QPixmap& px)
{
    m_tooth = px;
    update();
}
