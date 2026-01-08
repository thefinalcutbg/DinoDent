#include "RoundedFrame.h"
#include "View/Theme.h"
#include <QPainterPath>

RoundedFrame::RoundedFrame(QWidget* parent)
    : QFrame(parent)
{
    m_color = Theme::mainBackgroundColor;
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    Theme::applyShadow(this);
}

void RoundedFrame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(rect(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPen pen(m_color);
    pen.setCosmetic(true);
    pen.setWidth(2);
    p.setPen(pen);

    const qreal half = pen.widthF() * 0.5;

    QRectF r = rect();
    r.adjust(half, half, -half, -half);

    QPainterPath path;
    path.addRoundedRect(r, Theme::radius, Theme::radius);

    p.fillPath(path, Theme::sectionBackground);

    p.drawPath(path);

    if (m_separatorPosition != -1) {
        p.drawLine(m_separatorPosition, 0, m_separatorPosition, height());
    }
}
