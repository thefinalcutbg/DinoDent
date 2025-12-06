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

    auto font = this->font();
    font.setBold(true);
    setFont(font);

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

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(rect(), Theme::radius, Theme::radius);

    QColor normalBg(Theme::sectionBackground);
    QColor hoverBg(Theme::inactiveTabBG);

    QColor normalText(Theme::fontTurquoiseClicked);
    QColor hoverText(Theme::fontTurquoise);

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

    QColor textColor;
    textColor.setRed(mixChannel(normalText.red(), hoverText.red(), t));
    textColor.setGreen(mixChannel(normalText.green(), hoverText.green(), t));
    textColor.setBlue(mixChannel(normalText.blue(), hoverText.blue(), t));
    textColor.setAlpha(mixChannel(normalText.alpha(), hoverText.alpha(), t));

    painter.fillPath(path, background);

    QPen pen(Theme::border);
    pen.setCosmetic(true);
    pen.setWidth(2);
    painter.setPen(pen);

    int iconRectSide = clicked ? 35 : 40;

    if (!icon().isNull()) {
        icon().paint(&painter,
            QRect(width() / 2 - iconRectSide / 2,
                height() / 2 - iconRectSide / 2 - 10,
                iconRectSide,
                iconRectSide));
    }

    painter.setPen(QPen(textColor));
    painter.setFont(font());

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WordWrap);
    textOption.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    painter.drawText(QRectF(10, 100, width() - 20, 50), text(), textOption);
}

bool ShortcutTile::eventFilter(QObject* o, QEvent* e)
{
    if (o != this) {
        return false;
    }

    if (e->type() == QEvent::Enter) {
        hover = true;
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));

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
        QApplication::restoreOverrideCursor();
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
