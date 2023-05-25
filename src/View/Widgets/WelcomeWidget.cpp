#include "WelcomeWidget.h"
#include "View/Theme.h"

WelcomeWidget::WelcomeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.cornerLabel->setPixmap(QPixmap(":/icons/dinoSmall.png"));
	setStyleSheet("background-color:" + Theme::colorToString(Theme::background));
}

WelcomeWidget::~WelcomeWidget()
{}
