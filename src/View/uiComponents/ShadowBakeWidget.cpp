#include "ShadowBakeWidget.h"
#include <QApplication>
#include <QChildEvent>
#include <QEvent>
#include "View/Theme.h"
#include <QPainter>
#include <QTimer>

bool s_bakingEnabled = false;

ShadowBakeWidget::ShadowBakeWidget(QWidget* parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
}

void ShadowBakeWidget::setShadowTargets(const std::vector<QWidget*>& targets)
{
    for (QWidget* w : m_targets) {
        if (w) w->removeEventFilter(this);
    }

    m_targets = targets;

    for (QWidget* w : m_targets) {
        installFiltersFor(w);
    }

    scheduleBake();
    update();
}

void ShadowBakeWidget::disableBaking(bool disabled)
{
	m_bakeDisabled = disabled;
}

void ShadowBakeWidget::installFiltersFor(QWidget* w)
{
    if (!w) return;

    w->installEventFilter(this);

    const auto descendants = w->findChildren<QWidget*>();
    for (QWidget* d : descendants) {
        d->installEventFilter(this);
    }
}

void ShadowBakeWidget::removeDeadTargets()
{
    m_targets.erase(
        std::remove(m_targets.begin(), m_targets.end(), nullptr),
        m_targets.end()
    );
}

void ShadowBakeWidget::scheduleBake()
{
    if (m_bakeScheduled || !s_bakingEnabled || m_bakeDisabled) {
            return;
    }

    m_bakeScheduled = true;

    QTimer::singleShot(m_debounceMs, this, [this]() {
        m_bakeScheduled = false;
        bakeNow();
        });
}

void ShadowBakeWidget::setTargetsShadowEnabled(bool enabled)
{
    for (QWidget* w : m_targets) {
        if (!w) continue;

        if (enabled) {
            Theme::applyShadow(w);
        }
        else {
            w->setGraphicsEffect(nullptr);
        }

        w->update();
    }
}

void ShadowBakeWidget::bakeNow()
{
    removeDeadTargets();

    if (size().isEmpty() || m_targets.empty())
        return;

	m_baked = QPixmap();

    setTargetsShadowEnabled(true);

    update();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);

    const qreal dpr = devicePixelRatioF();
    QPixmap pm(size() * dpr);
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    render(&pm, QPoint(), QRegion(),
        QWidget::DrawWindowBackground | QWidget::DrawChildren);

       // --- Mask everything except m_targets (and their descendants) ---
    {
        QSet<QWidget*> keep;

        for (QWidget* t : m_targets) {
            if (!t) continue;
            keep.insert(t);

            const auto descendants = t->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
            // If you want ALL descendants (not just direct), use default findChildren without flags:
            // const auto descendants = t->findChildren<QWidget*>();
            // We'll do ALL descendants:
            for (QWidget* d : t->findChildren<QWidget*>())
                keep.insert(d);
        }

        QPainter p(&pm);
        p.setCompositionMode(QPainter::CompositionMode_Source);

        const auto all = findChildren<QWidget*>();

        for (QWidget* w : all) {
            if (!w || w == this) continue;
            if (!w->isVisible()) continue;

            if (keep.contains(w)) continue;

            QRect r(w->mapTo(this, QPoint(0, 0)), w->size());

            //Also clear shadows around non-target widgets
             r.adjust(-2, -2, 2, 2);

            r = r.intersected(rect());
            if (r.isEmpty()) continue;

            p.fillRect(r, Qt::transparent);
        }

        p.end();
    }
    // --- end masking ---

    setTargetsShadowEnabled(false);

    m_baked = pm;
    update();
}

void ShadowBakeWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter p(this);

    p.fillRect(rect(), Theme::background);

    if (!m_baked.isNull()) p.drawPixmap(0, 0, m_baked);
}

void ShadowBakeWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    scheduleBake();
}

void ShadowBakeWidget::childEvent(QChildEvent* e)
{
    QWidget::childEvent(e);

    if (e->added() || e->removed()) {
        scheduleBake();
    }
}

bool ShadowBakeWidget::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::Show: {
        if (!s_bakingEnabled) {
            s_bakingEnabled = true;
			scheduleBake();
            break;
        }
    }
    case QEvent::LayoutRequest:
    case QEvent::PolishRequest:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::PaletteChange:
    case QEvent::ContentsRectChange:
    case QEvent::ScreenChangeInternal:
    case QEvent::DevicePixelRatioChange:
        if (!m_targets.empty()) {
            scheduleBake();
        }
        break;
    default:
        break;
    }

    return QWidget::event(e);
}

bool ShadowBakeWidget::eventFilter(QObject* watched, QEvent* e)
{
    // If anything relevant changes on targets (or their descendants), rebake.
    switch (e->type()) {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Show:
    case QEvent::Hide:
    case QEvent::LayoutRequest:
    case QEvent::ZOrderChange:
    case QEvent::ParentChange:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::PaletteChange:
        if (!m_targets.empty()) {
            scheduleBake();
        }
        break;
    default:
        break;
    }

    return QWidget::eventFilter(watched, e);
}
