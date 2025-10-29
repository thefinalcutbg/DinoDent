#include "WelcomeWidget.h"
#include "View/Theme.h"
#include "Presenter/MainPresenter.h"
#include "View/Widgets/AboutDialog.h"
#include "View/Widgets/DinoDent.h"
#include <QDate>
#include <QDesktopServices>

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
    ui.calendar->setIcon(QIcon(":/icons/icon_calendar.png"));
    ui.aboutButton->setIcon(QIcon(":/icons/icon_question.png"));
    ui.donateButton->setIcon(QIcon(":/icons/icon_donate.png"));
    ui.ircButton->setIcon(QIcon(":/icons/icon_mirc.png"));

    connect(ui.ambButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newAmbPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newPerioPressed(); });
    connect(ui.prescrButton, &QPushButton::clicked, [&] { MainPresenter::get().newPrescriptionPressed(); });
    connect(ui.invoiceButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newInvoicePressed(); });
    connect(ui.browser, &QPushButton::clicked, this, [&] { MainPresenter::get().showBrowser(); });
    connect(ui.nhifButton, &QPushButton::clicked, this, [&] { MainPresenter::get().pisDialog(); });
    connect(ui.settingsButton, &QPushButton::clicked, this, [&] { MainPresenter::get().settingsPressed(); });
    connect(ui.statisticButton, &QPushButton::clicked, this, [&] { MainPresenter::get().statisticPressed(); });
    connect(ui.calendar, &QPushButton::clicked, this, [&] { MainPresenter::get().openCalendar(); });
    connect(ui.aboutButton, &QPushButton::clicked, this, [&] { AboutDialog d; d.exec();});
    connect(ui.donateButton, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://dinodent.bg/donate/", QUrl::TolerantMode)); });
    connect(ui.ircButton, &QPushButton::clicked, this, [&] { 

        QMainWindow* mainWin = qobject_cast<QMainWindow*>(QApplication::activeWindow());

        if (mainWin) {
            static_cast<DinoDent*>(mainWin)->openIrc();
        } 
    });
    
}

WelcomeWidget::~WelcomeWidget()
{}
