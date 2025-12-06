#include "ControlPanelPolygon.h"
#include <QPainter>
#include <QPainterPathStroker>
#include <QGraphicsSceneMouseEvent>
#include <QVariantAnimation>
#include <QEasingCurve>
#include "View/Theme.h"

ControlPanelPolygon::ControlPanelPolygon(ButtonPos position, PolygonObserver* observer)
    : QGraphicsObject(nullptr)
    , observer(observer)
    , position(position)
    , hovered(false)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    QPoint up_left_out(0, 0);
    QPoint down_left_out(0, 100);
    QPoint up_right_out(100, 0);
    QPoint down_right_out(100, 100);

    QPoint up_left_in(25, 25);
    QPoint down_left_in(25, 75);
    QPoint up_right_in(75, 25);
    QPoint down_right_in(75, 75);

    switch (position)
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
    case ButtonPos::center:
        poly << up_left_in << up_right_in << down_right_in << down_left_in;
    case ButtonPos::side:
        break;
    }

    hoverAnimation = new QVariantAnimation(this);
    hoverAnimation->setDuration(150);
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    hoverAnimation->setStartValue(0.0);
    hoverAnimation->setEndValue(0.0);

    connect(hoverAnimation, &QVariantAnimation::valueChanged, this,
        [this](const QVariant& v) {
            hoverProgress = v.toReal();
            update();
        });
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

    qreal t = hoverProgress;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    painter->setOpacity(0.1);
    painter->drawPolygon(poly);

    if (t > 0.0) {
        painter->setOpacity(0.3 * t);
        painter->fillPath(area, Qt::gray);
    }
}

void ControlPanelPolygon::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    hovered = true;

    if (hoverAnimation) {
        hoverAnimation->stop();
        hoverAnimation->setStartValue(hoverProgress);
        hoverAnimation->setEndValue(1.0);
        hoverAnimation->start();
    }
    else {
        hoverProgress = 1.0;
        update();
    }

    observer->buttonHovered(position, Hover::in);
}

void ControlPanelPolygon::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    hovered = false;

    if (hoverAnimation) {
        hoverAnimation->stop();
        hoverAnimation->setStartValue(hoverProgress);
        hoverAnimation->setEndValue(0.0);
        hoverAnimation->start();
    }
    else {
        hoverProgress = 0.0;
        update();
    }

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
    path.addRoundedRect(-5, -5, 110, 110, 12, 12);

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
