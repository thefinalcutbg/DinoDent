#include "SmoothWheelScroll.h"

#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QApplication>
#include <QWidget>
#include <algorithm>

SmoothWheelScroll::SmoothWheelScroll(QScrollArea* area, QObject* parent)
    : QObject(parent), m_area(area)
{
    auto* sb = m_area->verticalScrollBar();
    m_anim = new QPropertyAnimation(sb, "value", this);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
    m_anim->setDuration(180);
    m_target = sb->value();

    connect(sb, &QScrollBar::rangeChanged, this, [this](int, int) {
        if (!m_area) return;
        auto* sb2 = m_area->verticalScrollBar();
        m_anim->stop();
        m_target = sb2->value();
        });

    qApp->installEventFilter(this);
}

SmoothWheelScroll::~SmoothWheelScroll()
{
    if (qApp) qApp->removeEventFilter(this);
}

void SmoothWheelScroll::setPixelsPerStep(int px)
{
    m_pxPerStep = std::max(1, px);
}

void SmoothWheelScroll::setDurationMs(int ms)
{
    if (m_anim)
        m_anim->setDuration(std::max(1, ms));
}

bool SmoothWheelScroll::eventFilter(QObject* obj, QEvent* ev)
{
    if (!m_area) return QObject::eventFilter(obj, ev);
    if (ev->type() != QEvent::Wheel) return QObject::eventFilter(obj, ev);

    if (!m_area->isVisible() || !m_area->viewport()->isVisible())
        return QObject::eventFilter(obj, ev);

    auto* sb = m_area->verticalScrollBar();
    if (!sb || sb->maximum() <= sb->minimum())
        return QObject::eventFilter(obj, ev);

    auto* w = qobject_cast<QWidget*>(obj);
    if (!w) return QObject::eventFilter(obj, ev);
    if (qobject_cast<QScrollBar*>(w)) return QObject::eventFilter(obj, ev);

    auto* vp = m_area->viewport();
    if (!(w == vp || vp->isAncestorOf(w))) return QObject::eventFilter(obj, ev);

    auto* owner = nearestScrollArea(w);
    if (owner && owner != m_area) return QObject::eventFilter(obj, ev);

    auto* we = static_cast<QWheelEvent*>(ev);

    const QPoint ad = we->angleDelta();
    const int ay = ad.y();

    if (ay == 0) return QObject::eventFilter(obj, ev);

    if (we->phase() != Qt::NoScrollPhase)
        return QObject::eventFilter(obj, ev);

    const int absAy = std::abs(ay);
    if (absAy < 120 || (absAy % 120) != 0)
        return QObject::eventFilter(obj, ev);

    if (m_anim->state() != QAbstractAnimation::Running)
        m_target = sb->value();

    const int steps = ay / 120;
    m_target -= steps * m_pxPerStep;

    m_target = std::clamp(m_target, sb->minimum(), sb->maximum());

    m_anim->stop();
    m_anim->setStartValue(sb->value());
    m_anim->setEndValue(m_target);
    m_anim->start();

    we->accept();
    return true;
}

QAbstractScrollArea* SmoothWheelScroll::nearestScrollArea(QWidget* w)
{
    for (QWidget* p = w; p; p = p->parentWidget()) {
        if (auto* asa = qobject_cast<QAbstractScrollArea*>(p))
            return asa;
    }
    return nullptr;
}
