#include "ShadowBakeWidget.h"
#include "View/Theme.h"

#include <QApplication>
#include <QChildEvent>
#include <QEvent>
#include <QPainter>
#include <QTimer>

ShadowBakeWidget::ShadowBakeWidget(QWidget* parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
}

void ShadowBakeWidget::setShadowTargets(const std::vector<QWidget*>& targets)
{
    // Remove old filters
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
    if (m_bakeScheduled)
        return;

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
    case QEvent::LayoutRequest:
    case QEvent::PolishRequest:
    case QEvent::StyleChange:
    case QEvent::FontChange:
    case QEvent::PaletteChange:
    case QEvent::ContentsRectChange:
    case QEvent::ScreenChangeInternal:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    case QEvent::DevicePixelRatioChange:
#endif
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
