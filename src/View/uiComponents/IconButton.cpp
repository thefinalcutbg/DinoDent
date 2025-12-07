#include "IconButton.h"

#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include <QStatusBar>
#include <QApplication>
#include <QVariantAnimation>
#include <QEasingCurve>

#include "View/Theme.h"
#include "View/Widgets/GlobalWidgets.h"

IconButton::IconButton(QWidget* parent)
    : QPushButton(parent)
    , hoverColor(Theme::background)
    , backgroundColor(Theme::sectionBackground)
{
    installEventFilter(this);

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

    setCursor(QCursor(Qt::PointingHandCursor));

    m_hoverAnimation = new QVariantAnimation(this);
    m_hoverAnimation->setDuration(150); // ms
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_hoverAnimation->setStartValue(0.0);
    m_hoverAnimation->setEndValue(0.0);

    connect(m_hoverAnimation, &QVariantAnimation::valueChanged, this,
        [this](const QVariant& value) {
            m_hoverProgress = value.toReal();
            update();
        });
}

void IconButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF r = rect();
    QPainterPath path;
    path.addEllipse(r);

    QColor color = backgroundColor;

    if (isEnabled()) {
        qreal t = isChecked() ? 1.0 : m_hoverProgress;
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;

        auto mixChannel = [t](int a, int b) -> int {
            return a + qRound((b - a) * t);
            };

        color.setRed(mixChannel(backgroundColor.red(), hoverColor.red()));
        color.setGreen(mixChannel(backgroundColor.green(), hoverColor.green()));
        color.setBlue(mixChannel(backgroundColor.blue(), hoverColor.blue()));
        color.setAlpha(mixChannel(backgroundColor.alpha(), hoverColor.alpha()));
    }

    painter.fillPath(path, color);

    int padding = m_clicked && m_hover ? 15 : 12;

    QRect iconRect(rect().center(), QSize(rect().width() - padding, rect().height() - padding));
    iconRect.moveCenter(rect().center());

    QIcon::Mode iconMode = isEnabled() ? QIcon::Mode::Normal : QIcon::Mode::Disabled;

    if (m_monochrome) {
        iconMode = QIcon::Disabled;
    }

    if (!icon().isNull()) {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        icon().paint(&painter, iconRect, Qt::AlignCenter, iconMode);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    }

    if (outlineColor.isValid()) {
        QPen pen(outlineColor);
        pen.setCosmetic(true);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(r.adjusted(0.5, 0.5, -0.5, -0.5));
    }
}

bool IconButton::eventFilter(QObject*, QEvent* e)
{
    switch (e->type()) {
    case QEvent::Enter:
        m_hover = true;
        if (GlobalWidgets::statusBar) {
            GlobalWidgets::statusBar->showMessage(toolTip());
        }

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
        break;

    case QEvent::Leave:
        m_hover = false;

        if (GlobalWidgets::statusBar) {
            GlobalWidgets::statusBar->clearMessage();
        }

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
        break;

    case QEvent::MouseButtonPress:
        if (isEnabled()) {
            m_clicked = true;
            update();
        }
        break;

    case QEvent::MouseButtonRelease:
        if (isEnabled()) {
            m_clicked = false;
            update();
        }
        break;

    default:
        break;
    }

    return false;
}

void IconButton::setHoverColor(const QColor& color)
{
    hoverColor = color;
    update();
}

void IconButton::setBackgroundColor(const QColor& color)
{
    backgroundColor = color;
    update();
}

void IconButton::setMonochrome(bool monochrome)
{
    m_monochrome = monochrome;
    update();
}

IconButton::~IconButton() = default;
