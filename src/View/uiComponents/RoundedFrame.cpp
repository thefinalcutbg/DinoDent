#include "RoundedFrame.h"
#include "View/Theme.h"
#include <QPainterPath>
#include <QApplication>
#include <QAbstractScrollArea>

RoundedFrame::RoundedFrame(QWidget* parent)
    : QFrame(parent)
{
    m_color = Theme::mainBackgroundColor;

 //   Theme::applyShadow(this);
}

void RoundedFrame::setDynamicFocusBorderChange()
{
    const auto widgets = findChildren<QWidget*>();
    for (QWidget* w : widgets) {
        w->installEventFilter(this);

        // Important: QAbstractScrollArea focus often goes to viewport()
        if (auto* sa = qobject_cast<QAbstractScrollArea*>(w)) {
            if (sa->viewport())
                sa->viewport()->installEventFilter(this);
        }
    }

    updateFocusState();
}

void RoundedFrame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (!graphicsEffect()) {
        //return;
        //p.fillRect(rect(), Theme::background);
    }

    QPen pen(m_hasChildFocus ? m_color : Theme::border);
    pen.setCosmetic(true);
    pen.setWidth(2);
    p.setPen(pen);

    const qreal half = pen.widthF() * 0.5;
    QRectF r = rect();
    r.adjust(half, half, -half, -half);

    QPainterPath path;
    path.addRoundedRect(r, Theme::radius / 2, Theme::radius / 2);

    p.fillPath(path, Theme::sectionBackground);
    p.drawPath(path);

    if (m_separatorPosition != -1)
        p.drawLine(m_separatorPosition, 0, m_separatorPosition, height());
}

void RoundedFrame::updateFocusState()
{
    QWidget* nowW = QApplication::focusWidget();
    const bool inside = nowW && (nowW == this || isAncestorOf(nowW));
    if (inside == m_hasChildFocus) return;
    m_hasChildFocus = inside;
    update();
}

bool RoundedFrame::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
        updateFocusState();
    }
    return false;
}
