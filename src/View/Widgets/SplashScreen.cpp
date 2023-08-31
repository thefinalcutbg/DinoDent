#include "SplashScreen.h"

#include <QSplashScreen>
#include <QApplication>

QSplashScreen* s_splash{nullptr};

void SplashScreen::createAndShow()
{
	s_splash = new QSplashScreen(QPixmap(":/other/splash.png"));
    s_splash->show();
    s_splash->showMessage("DinoDent зарежда, моля изчакайте...", Qt::AlignLeft | Qt::AlignBottom, Qt::darkGray);
    QApplication::instance()->processEvents();
}

void SplashScreen::showMessage(const std::string& message)
{
    if (s_splash == nullptr) return;

    s_splash->showMessage(message.c_str(), Qt::AlignLeft | Qt::AlignBottom, Qt::darkGray);
}

void SplashScreen::hideAndDestroy()
{
    if (s_splash == nullptr) return;

    s_splash->hide();
    
    delete s_splash;
    
    s_splash = nullptr;
}