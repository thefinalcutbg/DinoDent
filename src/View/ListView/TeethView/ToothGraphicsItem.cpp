#include "ToothGraphicsItem.h"
#include <QPainter>
#include <QIcon>
#include <QGraphicsSceneMouseEvent>

ToothGraphicsItem::ToothGraphicsItem(int index) : index(index), hasProcedure(false), hasNote(false)
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
    static QPixmap note("note.png");

    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    if (hasProcedure)
    {
        painter->setOpacity(0.2);
        painter->fillRect(0, procedureMarkerHeight, bounds.width(), 15, QBrush(Qt::green));
        painter->setOpacity(1);
    }

    if (hasNote)
    { 
        QRect noteRect(2, procedureMarkerHeight, 15, 15);
        painter->drawPixmap(noteRect, note);
    }

    int xPos = (bounds.width() / 2) - (pxWidth / 2);

    painter->drawPixmap(xPos, 0, pxWidth, pxHeight, m_tooth);
    
    if (bounds.width() > pxWidth)
    {
        painter->drawPixmap(QRect(0, 0, xPos, pxHeight), m_tooth, QRect(0, 0, 1, m_tooth.height()));
        
        painter->drawPixmap(QRect(xPos+pxWidth, 0, xPos, pxHeight), m_tooth, QRect(m_tooth.width() - 1, 0, 1, m_tooth.height()));
    }

   // QColor color(Qt::GlobalColor::black);
   // painter->setPen(color);
   // painter->drawRect(bounds);

    //painter->setOpacity(0.3);
    //painter->fillRect(bounds, QBrush(Qt::green));
    
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

void ToothGraphicsItem::setNote(bool hasNote)
{
    this->hasNote = hasNote;
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
