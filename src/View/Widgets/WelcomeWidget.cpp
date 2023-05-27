#include "WelcomeWidget.h"
#include "View/Theme.h"
#include "Presenter/MainPresenter.h"
#include "View/Widgets/AboutDialog.h"
#include <QDate>
WelcomeWidget::WelcomeWidget(MainPresenter& p, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    ui.cornerLabel->setPixmap(
        QDate::currentDate().month() == 12 ?
        QPixmap(":/icons/dinoChristmas.png")
        :
        QPixmap(":/icons/dinoSmall.png")
    );

	setStyleSheet("background-color:" + Theme::colorToString(Theme::background));

    ui.ambButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.prescrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
    ui.browser->setIcon(QIcon(":/icons/icon_open.png"));
    ui.nhifButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));
    ui.statisticButton->setIcon(QIcon(":/icons/icon_statistic.png"));
    ui.aboutButton->setIcon(QIcon(":/icons/icon_question.png"));

    connect(ui.ambButton, &QPushButton::clicked, [&] { p.newAmbPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] { p.newPerioPressed(); });
    connect(ui.prescrButton, &QPushButton::clicked, [&] { p.newPerscriptionPressed(); });
    connect(ui.browser, &QPushButton::clicked, [&] { p.showListSelector(); });
    connect(ui.nhifButton, &QPushButton::clicked, [&] { p.pisDialog(); });
    connect(ui.settingsButton, &QPushButton::clicked, [&] { p.settingsPressed(); });
    connect(ui.statisticButton, &QPushButton::clicked, [&] { p.statisticPressed(); });
    connect(ui.aboutButton, &QPushButton::clicked, [&] { AboutDialog d; d.exec(); });
    
}

WelcomeWidget::~WelcomeWidget()
{}
