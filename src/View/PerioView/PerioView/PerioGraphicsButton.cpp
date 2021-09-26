#include "PerioGraphicsButton.h"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>

PerioGraphicsButton::PerioGraphicsButton(ButtonPos position, PerioView* perioView)
{
    this->view = perioView;

    QPointF upLeft(0, 0); QPointF downLeft(0, bounds.height());
    QPointF upRight(bounds.width(), 0); QPointF downRight(bounds.width(), bounds.height());
    QPointF center(bounds.width() / 2, bounds.height() / 2);

    switch (position)
    {
        case ButtonPos::left: surface << upLeft << center << downLeft; break;
        case ButtonPos::up: surface << upLeft << center << upRight; break;
        case ButtonPos::right: surface << upRight << center << downRight; break;
        case ButtonPos::down:surface << downLeft << center << downRight; break;
    }
    
    path.addPolygon(surface);

    setAcceptHoverEvents(true);
}





void PerioGraphicsButton::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_disabled)
    {
        painter->fillPath(path, QBrush{ QColor{236, 236, 236} });
    }
    else if (m_checked)
    {
        painter->fillPath(path, QBrush(m_checkedColor));
    }


    if (m_hovered && !m_disabled)
    {
        painter->setOpacity(0.3);
        QColor color(Qt::GlobalColor::gray);
        painter->fillPath(path, QBrush(color));
        painter->setOpacity(1);
    }


    QPen pen;
    pen.setColor(Qt::GlobalColor::darkGray);
    pen.setWidthF(1);
    painter->setPen(pen);

    painter->drawPath(path);

    painter->drawRect(bounds);
}

void PerioGraphicsButton::setColor(QColor color) { m_checkedColor = color; update(); }
void PerioGraphicsButton::setDisabled(bool disabled) { m_disabled = disabled; update(); }
bool PerioGraphicsButton::isEnabled() { return !m_disabled; }
void PerioGraphicsButton::hoverEnterEvent(QGraphicsSceneHoverEvent* event) { m_hovered = true; update();}
void PerioGraphicsButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) { m_hovered = false; update();}
void PerioGraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent* event) { m_checked = !m_checked; update(); }
QRectF PerioGraphicsButton::boundingRect() const { return bounds; }
QPainterPath PerioGraphicsButton::shape() const { return path; }
