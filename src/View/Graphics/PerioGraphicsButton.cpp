#include "PerioGraphicsButton.h"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include "View/Widgets/PerioView.h"

PerioGraphicsButton::PerioGraphicsButton(int index, PerioGraphicsType type, PerioView* perioView)
{
    this->view = perioView;
    m_index = index;
    this->m_type = type;

    switch (type)
    {
        case PerioGraphicsType::Plaque: m_checkedColor.setRgb(204, 228, 247); break;
        case PerioGraphicsType::Bleeding: m_checkedColor.setRgb(255, 146, 148); break;
    }

    QPointF upLeft(0, 0); QPointF downLeft(0, bounds.height());
    QPointF upRight(bounds.width(), 0); QPointF downRight(bounds.width(), bounds.height());
    QPointF center(bounds.width() / 2, bounds.height() / 2);

    enum position{left, up, right, down};

    switch (index%4)
    {
        case left: surface << upLeft << center << downLeft; break;
        case up: surface << upLeft << center << upRight; break;
        case right: surface << upRight << center << downRight; break;
        case down:surface << downLeft << center << downRight; break;
    }

    
    path.addPolygon(surface);

    setAcceptHoverEvents(true);
}





void PerioGraphicsButton::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    painter->setRenderHint(QPainter::Antialiasing);

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
    pen.setCosmetic(true);
    painter->setPen(pen);

    painter->drawPath(path);

    painter->drawRect(bounds);
}

QRectF PerioGraphicsButton::boundingRect() const { return bounds; }
QPainterPath PerioGraphicsButton::shape() const { return path; }
void PerioGraphicsButton::setDisabled(bool disabled) { m_disabled = disabled; update(); }
bool PerioGraphicsButton::isEnabled() { return !m_disabled; }
bool PerioGraphicsButton::isChecked() { return m_checked;  }
void PerioGraphicsButton::setChecked(bool checked) { m_checked = checked; update(); }
void PerioGraphicsButton::hoverEnterEvent(QGraphicsSceneHoverEvent* event) { m_hovered = true; update();}
void PerioGraphicsButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) { m_hovered = false; update();}
void PerioGraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent* event) 
{ 
    if (m_disabled) return;
    setChecked(m_checked = !m_checked);
    view->PerioGraphicClicked(m_index, m_type);
}


