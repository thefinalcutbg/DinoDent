#include "ControlPanelPolygon.h"
#include <QPainter>
#include <QPainterPathStroker>
#include <QGraphicsSceneMouseEvent>
#include "View/Theme.h"

ControlPanelPolygon::ControlPanelPolygon(ButtonPos position, PolygonObserver* observer)
    : observer(observer), position(position), hovered(false)
{
    this->setFlag(QGraphicsItem::ItemIsFocusable);
    this->setAcceptHoverEvents(true);

    QPoint up_left_out(0, 0);   //outer points of the CP
    QPoint down_left_out(0, 100);
    QPoint up_right_out(100, 0);
    QPoint down_right_out(100, 100);

    QPoint up_left_in(25, 25);  //inner points of the CP
    QPoint down_left_in(25, 75);
    QPoint up_right_in(75, 25);
    QPoint down_right_in(75, 75);


    switch (position) //setting the coordinates for the drawing of the trapezium, depending on it's orientation
    {
    case ButtonPos::left:
        poly << up_left_out << up_left_in << down_left_in << down_left_out;
        break;
    case ButtonPos::up:
        poly << up_left_in << up_right_in << up_right_out << up_left_out;
        break;
    case ButtonPos::right:
        poly << up_right_in << up_right_out << down_right_out << down_right_in;
        break;
    case ButtonPos::down:
        poly << down_left_in << down_right_in << down_right_out << down_left_out;
        break;
    case ButtonPos::center:                          //here we draw a square
        poly << up_left_in << up_right_in << down_right_in << down_left_in;
    case ButtonPos::side:
        break;
    }
}

QRectF ControlPanelPolygon::boundingRect() const
{
    return QRect(0, 0, 100, 100);
}

QPainterPath ControlPanelPolygon::shape() const
{
    QPainterPath area;
    area.addPolygon(poly);
    return area;
}

void ControlPanelPolygon::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPainterPath area;
    area.addPolygon(poly);
    painter->setOpacity(0.1);
    painter->drawPolygon(poly);


    if (hovered)
    {
        painter->setOpacity(0.3);
        painter->fillPath(area, Qt::gray);
    }

}

void ControlPanelPolygon::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    hovered = 1;
    this->update();
    observer->buttonHovered(position, Hover::in);
}

void ControlPanelPolygon::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    hovered = 0;
    this->update();
    observer->buttonHovered(position, Hover::out);
}

void ControlPanelPolygon::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    if (event->buttons() == Qt::LeftButton)
        observer->buttonClicked(this->position, MouseClick::leftClick);

    else if (event->buttons() == Qt::RightButton)
        observer->buttonClicked(this->position, MouseClick::rightClick);
}

QRectF PolygonBorder::boundingRect() const
{
    return QRectF(0, 0, 100, 100);
}

void PolygonBorder::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    QPainterPath path;
    path.addRoundedRect(-5,-5, 110, 110, 12, 12);

    auto pen = painter->pen();
    pen.setWidth(10);
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->drawPath(path);

    path.clear();

    path.addRoundedRect(boundingRect(), 8, 8);
    painter->setPen(is_focused ? QPen(Theme::mainBackgroundColor) : QPen(Theme::border));
    painter->drawPath(path);

}
