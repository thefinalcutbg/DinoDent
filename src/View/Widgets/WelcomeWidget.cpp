#include "WelcomeWidget.h"
#include "View/Theme.h"
#include "Presenter/MainPresenter.h"
#include "View/Widgets/AboutDialog.h"
#include <QDate>

WelcomeWidget::WelcomeWidget(QWidget *parent)
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
    ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
    ui.browser->setIcon(QIcon(":/icons/icon_open.png"));
    ui.nhifButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));
    ui.statisticButton->setIcon(QIcon(":/icons/icon_statistic.png"));
    ui.aboutButton->setIcon(QIcon(":/icons/icon_question.png"));

    connect(ui.ambButton, &QPushButton::clicked, [&] { MainPresenter::get().newAmbPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] { MainPresenter::get().newPerioPressed(); });
    connect(ui.prescrButton, &QPushButton::clicked, [&] { MainPresenter::get().newPerscriptionPressed(); });
    connect(ui.invoiceButton, &QPushButton::clicked, [&] { MainPresenter::get().newInvoicePressed(); });
    connect(ui.browser, &QPushButton::clicked, [&] { MainPresenter::get().showBrowser(); });
    connect(ui.nhifButton, &QPushButton::clicked, [&] { MainPresenter::get().pisDialog(); });
    connect(ui.settingsButton, &QPushButton::clicked, [&] { MainPresenter::get().settingsPressed(); });
    connect(ui.statisticButton, &QPushButton::clicked, [&] { MainPresenter::get().statisticPressed(); });
    connect(ui.aboutButton, &QPushButton::clicked, [&] { AboutDialog d; d.exec(); });
    
}

WelcomeWidget::~WelcomeWidget()
{}
