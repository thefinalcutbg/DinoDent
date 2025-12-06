#include "SelectionBox.h"
#include "View/Theme.h"
#include <QVariantAnimation>
#include <QEasingCurve>

SelectionBox::SelectionBox(int index)
    : QGraphicsObject(nullptr)
    , index(index)
    , hovered(false)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    bounds.setHeight(200);
    if ((index > 2 && index < 13) || (index > 18 && index < 29)) {
        bounds.setWidth(36);
    }
    else {
        bounds.setWidth(54);
    }

    m_hoverAnimation = new QVariantAnimation(this);
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_hoverAnimation->setStartValue(0.0);
    m_hoverAnimation->setEndValue(0.0);
    connect(m_hoverAnimation, &QVariantAnimation::valueChanged, this,
        [this](const QVariant& v) {
            m_hoverProgress = v.toReal();
            update();
        });
}

SelectionBox::~SelectionBox()
{
}

QRectF SelectionBox::boundingRect() const
{
    return bounds;
}

void SelectionBox::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto mixChannel = [](int a, int b, qreal t) -> int {
        return a + qRound((b - a) * t);
        };

    qreal h = m_hoverProgress;
    if (h < 0.0) h = 0.0;
    if (h > 1.0) h = 1.0;

    qreal s = isSelected() ? 1.0 : 0.0;

    QColor baseBorder = Theme::border;
    QColor selectBorder = m_focused ? Theme::mainBackgroundColor : Theme::border;

    qreal hoverAlpha = 0.1 * h * (1.0 - s);
    qreal selectAlpha = 0.1 * s;
    qreal totalAlpha = hoverAlpha + selectAlpha;
    if (totalAlpha > 0.1)
        totalAlpha = 0.1;

    qreal tSel = s;

    QColor outlineColor;
    outlineColor.setRed(mixChannel(baseBorder.red(), selectBorder.red(), tSel));
    outlineColor.setGreen(mixChannel(baseBorder.green(), selectBorder.green(), tSel));
    outlineColor.setBlue(mixChannel(baseBorder.blue(), selectBorder.blue(), tSel));
    outlineColor.setAlphaF(qBound(0.0, totalAlpha / 0.1, 1.0));

    QPen pen;
    pen.setColor(outlineColor);
    pen.setWidth(1);
    painter->setPen(pen);

    QColor fillColor(Qt::gray);
    fillColor.setAlphaF(totalAlpha);

    painter->drawRect(bounds);
    painter->fillRect(bounds, fillColor);
}

void SelectionBox::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    hovered = true;

    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(1.0);
        m_hoverAnimation->start();
    }
    else {
        m_hoverProgress = 1.0;
        update();
    }
}

void SelectionBox::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    hovered = false;

    if (m_hoverAnimation) {
        m_hoverAnimation->stop();
        m_hoverAnimation->setStartValue(m_hoverProgress);
        m_hoverAnimation->setEndValue(0.0);
        m_hoverAnimation->start();
    }
    else {
        m_hoverProgress = 0.0;
        update();
    }
}

int SelectionBox::getIndex()
{
    return this->index;
}

void SelectionBox::drawFocused(bool focused)
{
    m_focused = focused;
    update();
}
