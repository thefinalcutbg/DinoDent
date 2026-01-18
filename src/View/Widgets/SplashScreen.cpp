#include "SplashScreen.h"

#include <QSplashScreen>
#include <QApplication>
#include <QPainter>
#include "View/Theme.h"

QSplashScreen* s_splash{nullptr};

void SplashScreen::createAndShow()
{
    auto background = QPixmap(":/other/splash.png");
	auto centerImage = Theme::getDinoSprite();

	auto splashImage = QPixmap(background.size());
	splashImage.fill(Qt::transparent);

    QPainter painter(&splashImage);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter.drawPixmap(0, 0, background);

    int padX = qMax(0, background.width() / 6);
    int padY = qMax(0, background.height() / 6);

    const int bottomKeepout = qMax(padY, background.height() / 5);

    QRect inner = QRect(QPoint(0, 0), background.size())
        .adjusted(padX, padY, -padX, -(bottomKeepout));

    QSize target = centerImage.size();

    target.scale(inner.size(), Qt::KeepAspectRatio);

    const int x = inner.x() + (inner.width() - target.width()) / 2;
    int y = inner.y() + (inner.height() - target.height()) / 2;

    const int maxY = inner.bottom() - target.height() + 1;
    if (y > maxY) y = maxY;

    
    painter.drawPixmap(QRect(x, y, target.width(), target.height()), centerImage);
   
    s_splash = new QSplashScreen(splashImage);
    s_splash->show();
    s_splash->showMessage("  DinoDent зарежда, моля изчакайте...", Qt::AlignLeft | Qt::AlignBottom, Qt::darkGray);
    QApplication::instance()->processEvents();
}

void SplashScreen::showMessage(const std::string& message)
{
    if (s_splash == nullptr) return;

    s_splash->showMessage(QString("  ") + message.c_str(), Qt::AlignLeft | Qt::AlignBottom, Qt::darkGray);
}

void SplashScreen::hideAndDestroy()
{
    if (s_splash == nullptr) return;

    s_splash->hide();
    
    delete s_splash;
    
    s_splash = nullptr;
}
