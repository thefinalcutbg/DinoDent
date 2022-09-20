#include "SelectionBox.h"

SelectionBox::SelectionBox(int index) : hovered(false), index(index)
{
	setFlag(QGraphicsItem::ItemIsSelectable);
	setAcceptHoverEvents(true);

	bounds.setHeight(200);
    if (index > 2 && index < 13 || index > 18 && index < 29) {
        bounds.setWidth(36);
    }
    else {
        bounds.setWidth(54);
    }
		
}

SelectionBox::~SelectionBox()
{
}

QRectF SelectionBox::boundingRect() const
{
	return bounds;
}


void SelectionBox::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    if (this->isSelected())  //painting the selection box
    {
        QColor select(Qt::gray);
        QPen linepen;
        linepen.setWidth(2);
        linepen.setColor(select);
        linepen.setWidth(1); painter->setPen(linepen);
        painter->drawRect(bounds);
        painter->setOpacity(0.1);
        QBrush selectb(Qt::gray, Qt::SolidPattern);
        painter->fillRect(bounds, selectb);
    }
    else if (hovered)
    {
        QColor select = Qt::gray;
        painter->setOpacity(0.1);
        painter->drawRect(bounds);
        QBrush selectb(select, Qt::SolidPattern);
        painter->fillRect(bounds, selectb);
    }
}


void SelectionBox::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    hovered = 1;
    this->update();
}

void SelectionBox::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    hovered = 0;
    this->update();
}


int SelectionBox::getIndex()
{
    return this->index;
}
