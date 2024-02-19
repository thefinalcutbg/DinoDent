#include "WelcomeWidget.h"
#include "View/Theme.h"
#include "Presenter/MainPresenter.h"
#include "View/Widgets/AboutDialog.h"
#include <QDate>

WelcomeWidget::WelcomeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    auto date = Date::currentDate();

    if (date.month == 12)
    {
        ui.cornerLabel->setPixmap(QPixmap(":/icons/dinoChristmas.png"));
    }
    else if (date.isOrthodoxEasternWeek())
    {
        ui.cornerLabel->setPixmap(QPixmap(":/icons/dinoEaster.png"));
    }
    else
    {
        ui.cornerLabel->setPixmap(QPixmap(":/icons/dinoSmall.png"));
    }

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

    connect(ui.ambButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newAmbPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newPerioPressed(); });
    connect(ui.prescrButton, &QPushButton::clicked, [&] { MainPresenter::get().newPrescriptionPressed(); });
    connect(ui.invoiceButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newInvoicePressed(); });
    connect(ui.browser, &QPushButton::clicked, this, [&] { MainPresenter::get().showBrowser(); });
    connect(ui.nhifButton, &QPushButton::clicked, this, [&] { MainPresenter::get().pisDialog(); });
    connect(ui.settingsButton, &QPushButton::clicked, this, [&] { MainPresenter::get().settingsPressed(); });
    connect(ui.statisticButton, &QPushButton::clicked, this, [&] { MainPresenter::get().statisticPressed(); });
    connect(ui.aboutButton, &QPushButton::clicked, this, [&] { AboutDialog d; d.exec(); });
    
}

WelcomeWidget::~WelcomeWidget()
{}
