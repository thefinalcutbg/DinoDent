#include "SmoothWheelScroll.h"

#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QWidget>
#include <QEasingCurve>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>
#include <algorithm>
#include <cmath>

/* WRITTEN BY CHATGPT 5.2 */

SmoothWheelScroll::SmoothWheelScroll(QScrollArea* area, QObject* parent)
    : QObject(parent), m_area(area)
{
    auto* sb = m_area->verticalScrollBar();

    m_anim = new QPropertyAnimation(sb, "value", this);
    m_anim->setEasingCurve(QEasingCurve::OutSine);
    m_target = sb->value();

    connect(sb, &QScrollBar::rangeChanged, this, [this](int, int) {
        if (!m_area) return;
        auto* sb2 = m_area->verticalScrollBar();
        m_anim->stop();
        m_target = sb2->value();
        });

    m_repeatTimer = new QTimer(this);
    m_repeatTimer->setInterval(40);
    connect(m_repeatTimer, &QTimer::timeout, this, [this]() {
        if (!m_area) return;
        if (m_repeatDir == 0) return;
        auto* sb = m_area->verticalScrollBar();
        addToTarget(m_repeatDir * sb->singleStep());
        animateToTarget();
        });

    m_area->viewport()->installEventFilter(this);
    sb->installEventFilter(this);

}

void SmoothWheelScroll::setEnabled(bool on) { m_enabled = on; }
bool SmoothWheelScroll::isEnabled() const { return m_enabled; }

SmoothWheelScroll::~SmoothWheelScroll()
{

}

void SmoothWheelScroll::setPixelsPerWheelStep(int px)
{
    m_pxPerWheelStep = std::max(1, px);
}

void SmoothWheelScroll::setDurationRangeMs(int minMs, int maxMs)
{
    m_minMs = std::max(1, minMs);
    m_maxMs = std::max(m_minMs, maxMs);
}

void SmoothWheelScroll::setBaseDurationMs(int baseMs)
{
    m_baseMs = std::max(1, baseMs);
}

void SmoothWheelScroll::setDistanceDivisor(int divisor)
{
    m_distDiv = std::max(1, divisor);
}

bool SmoothWheelScroll::eventFilter(QObject* obj, QEvent* ev)
{
    if (!m_enabled) return QObject::eventFilter(obj, ev);

    if (!m_area) return QObject::eventFilter(obj, ev);

    auto* sb = m_area->verticalScrollBar();

    if (obj == sb) {
        if (ev->type() == QEvent::MouseButtonPress || 
            ev->type() == QEvent::MouseButtonDblClick) {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->button() != Qt::LeftButton) return QObject::eventFilter(obj, ev);

            QStyleOptionSlider opt;
            opt.initFrom(sb);
            opt.subControls = QStyle::SC_All;
            opt.orientation = sb->orientation();
            opt.minimum = sb->minimum();
            opt.maximum = sb->maximum();
            opt.sliderPosition = sb->sliderPosition();
            opt.sliderValue = sb->value();
            opt.singleStep = sb->singleStep();
            opt.pageStep = sb->pageStep();
            opt.upsideDown = sb->invertedAppearance();

            const QStyle::SubControl sc =
                sb->style()->hitTestComplexControl(QStyle::CC_ScrollBar, &opt, me->pos(), sb);

            if (sb->orientation() == Qt::Vertical) {
                if (sc == QStyle::SC_ScrollBarSubLine) {
                    stopArrowRepeat();
                    addToTarget(-sb->singleStep());
                    animateToTarget();
                    startArrowRepeat(-1);
                    me->accept();
                    return true;
                }
                if (sc == QStyle::SC_ScrollBarAddLine) {
                    stopArrowRepeat();
                    addToTarget(+sb->singleStep());
                    animateToTarget();
                    startArrowRepeat(+1);
                    me->accept();
                    return true;
                }
            }
        }

        if (ev->type() == QEvent::MouseButtonRelease) {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->button() == Qt::LeftButton) stopArrowRepeat();
        }

        if (ev->type() == QEvent::Leave) {
            stopArrowRepeat();
        }

        return QObject::eventFilter(obj, ev);
    }

    if (ev->type() != QEvent::Wheel) return QObject::eventFilter(obj, ev);

    if (!m_area->isVisible() || !m_area->viewport()->isVisible())
        return QObject::eventFilter(obj, ev);

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

    const int ay = we->angleDelta().y();
    if (ay == 0) return QObject::eventFilter(obj, ev);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (we->phase() != Qt::NoScrollPhase)
        return QObject::eventFilter(obj, ev);
#endif

    const int absAy = std::abs(ay);
    if (absAy < 120 || (absAy % 120) != 0)
        return QObject::eventFilter(obj, ev);

    const int steps = ay / 120;
    addToTarget(-steps * m_pxPerWheelStep);
    animateToTarget();

    we->accept();
    return true;
}

void SmoothWheelScroll::addToTarget(int delta)
{
    if (!m_area) return;
    auto* sb = m_area->verticalScrollBar();
    if (!sb) return;

    if (m_anim->state() != QAbstractAnimation::Running)
        m_target = sb->value();

    m_target += delta;
    m_target = std::clamp(m_target, sb->minimum(), sb->maximum());
}

void SmoothWheelScroll::animateToTarget()
{
    if (!m_area) return;
    auto* sb = m_area->verticalScrollBar();
    if (!sb) return;

    const int current = sb->value();
    const int dist = std::abs(m_target - current);
    const int dur = std::clamp(m_baseMs + dist / m_distDiv, m_minMs, m_maxMs);

    m_anim->stop();
    m_anim->setStartValue(current);
    m_anim->setEndValue(m_target);
    m_anim->setDuration(dur);
    m_anim->start();
}

void SmoothWheelScroll::startArrowRepeat(int dir)
{
    m_repeatDir = dir;
    if (!m_repeatTimer) return;
    if (m_repeatTimer->isActive()) return;

    QTimer::singleShot(250, this, [this]() {
        if (m_repeatDir != 0 && m_repeatTimer)
            m_repeatTimer->start();
        });
}

void SmoothWheelScroll::stopArrowRepeat()
{
    m_repeatDir = 0;
    if (m_repeatTimer) m_repeatTimer->stop();
}

QAbstractScrollArea* SmoothWheelScroll::nearestScrollArea(QWidget* w)
{
    for (QWidget* p = w; p; p = p->parentWidget()) {
        if (auto* asa = qobject_cast<QAbstractScrollArea*>(p))
            return asa;
    }
    return nullptr;
}
