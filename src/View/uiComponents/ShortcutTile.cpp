#include "ShortcutTile.h"

#include <QEvent>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QTextOption>
#include <QVariantAnimation>
#include <QEasingCurve>

#include "View/Theme.h"

ShortcutTile::ShortcutTile(QWidget* parent) : QPushButton(parent)
{
    installEventFilter(this);
    Theme::applyLightShadow(this);
    auto font = this->font();
    font.setBold(true);
    setFont(font);
    setCursor(Qt::PointingHandCursor);

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

void ShortcutTile::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

   // painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPen pen(Theme::border);
    pen.setCosmetic(true);
    pen.setWidth(2);
    painter.setPen(pen);

    const qreal half = pen.widthF() * 0.5;
    QRectF r = rect();
    r.adjust(half, half, -half, -half);

    QPainterPath path;
    path.addRoundedRect(r, Theme::radius / 2, Theme::radius / 2);

    QColor normalBg(Theme::sectionBackground);
    QColor hoverBg(Theme::inactiveTabBG);

    auto mixChannel = [](int a, int b, qreal t) -> int {
        return a + qRound((b - a) * t);
        };

    qreal t = hoverProgress;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    QColor background;
    background.setRed(mixChannel(normalBg.red(), hoverBg.red(), t));
    background.setGreen(mixChannel(normalBg.green(), hoverBg.green(), t));
    background.setBlue(mixChannel(normalBg.blue(), hoverBg.blue(), t));
    background.setAlpha(mixChannel(normalBg.alpha(), hoverBg.alpha(), t));

    painter.fillPath(path, background);

    painter.drawPath(path);

    int iconRectSide = clicked ? 30 : 35;//35 : 40;

    if (!icon().isNull()) {
        icon().paint(&painter,
            QRect(width() / 2 - iconRectSide / 2,
                height() / 2 - iconRectSide / 2, //-10
                iconRectSide,
                iconRectSide));
    }

}

bool ShortcutTile::eventFilter(QObject* o, QEvent* e)
{
    if (o != this) {
        return false;
    }

    if (e->type() == QEvent::Enter) {
        hover = true;

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

        hover = false;

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
        clicked = true;
    }

    if (e->type() == QEvent::MouseButtonRelease) {
        clicked = false;
    }

    return false;
}
