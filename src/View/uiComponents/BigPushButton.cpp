#include "BigPushButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include <QMenu>
#include <QVariantAnimation>
#include <QEasingCurve>

BigPushButton::BigPushButton(QWidget* parent) :
    QPushButton(parent),
    normalColor(Theme::sectionBackground),
    hoverColor(Theme::background)
{
    this->installEventFilter(this);

    setCursor(QCursor(Qt::PointingHandCursor));

    auto font = this->font();
    font.setBold(true);
    setFont(font);

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

    // Smooth hover color animation
    hoverAnimation = new QVariantAnimation(this);
    hoverAnimation->setDuration(150); // ms
    hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    hoverAnimation->setStartValue(0.0);
    hoverAnimation->setEndValue(0.0);

    connect(hoverAnimation, &QVariantAnimation::valueChanged,
        this, [this](const QVariant& value) {
            hoverProgress = value.toReal();
            update();
        });
}

void BigPushButton::setHoverColor(const QColor& color)
{
    this->hoverColor = color;
    update();
}

void BigPushButton::setNormalColor(const QColor& color)
{
    normalColor = color;
    update();
}

void BigPushButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addRoundedRect(QRectF(rect()), Theme::radius - 3, Theme::radius - 3);

    // Interpolate background color based on hover animation
    auto mixChannel = [](int a, int b, qreal t) -> int {
        return a + qRound((b - a) * t);
        };

    qreal t = isChecked() ? 1.0 : hoverProgress; // checked = fully "hovered"
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    QColor background;
    background.setRed(mixChannel(normalColor.red(), hoverColor.red(), t));
    background.setGreen(mixChannel(normalColor.green(), hoverColor.green(), t));
    background.setBlue(mixChannel(normalColor.blue(), hoverColor.blue(), t));
    background.setAlpha(mixChannel(normalColor.alpha(), hoverColor.alpha(), t));

    painter.fillPath(path, background);

    int iconSize = 18;
    QRect iconRect(10, (height() - iconSize) / 2, iconSize, iconSize);

    if (!icon().isNull())
        icon().paint(&painter, iconRect);

    // If colors are the same, we still want a font change on hover
    bool differentFont = (hoverColor == normalColor && hoverProgress > 0.0);

    painter.setFont(font());
    painter.setPen(QPen(differentFont ? Theme::fontTurquoise : Theme::fontTurquoiseClicked));

    int xPos = icon().isNull() ? 0 : 18;

    QRect textRect;
    textRect.setRect(xPos, 0, width() - xPos, height());

    painter.drawText(textRect, Qt::AlignCenter, text());
}

bool BigPushButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
        m_hover = true;

        // Animate hover in
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
    }

    if (e->type() == QEvent::Leave) {

        m_hover = false;

        // Animate hover out
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
    }

    if (e->type() == QEvent::MouseButtonPress) {
        // Preserve original behavior: cancel hover on press
        m_hover = false;

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
    }

    return false;
}

QSize BigPushButton::minimumSizeHint() const
{
    if (icon().isNull()) return QPushButton::minimumSizeHint();

    auto size = QPushButton::minimumSizeHint();
    size.setWidth(size.width() + 18);

    return size;
}

BigPushButton::~BigPushButton()
{
}
