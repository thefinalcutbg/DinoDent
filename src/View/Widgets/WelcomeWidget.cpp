#include "WelcomeWidget.h"

#include <QDate>
#include <QDesktopServices>
#include <QPainter>
#include <QFile>
#include <QRandomGenerator>
#include <QNetworkReply>

#include "Network/NetworkManager.h"
#include "Presenter/MainPresenter.h"
#include "View/Widgets/AboutDialog.h"
#include "View/Widgets/DinoDent.h"
#include "View/Theme.h"

void WelcomeWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
	painter.fillRect(event->rect(), Theme::background);
}

WelcomeWidget::WelcomeWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    setStyleSheet("color: " + Theme::colorToString(Theme::fontTurquoise));
    ui.tipLabel->setStyleSheet("color:rgb(68,68,68)");

    auto date = Date::currentDate();

    ui.cornerLabel->setPixmap(Theme::getDinoSprite());

    ui.ambButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.prescrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
    ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
    ui.browser->setIcon(QIcon(":/icons/icon_open.png"));
    ui.nhifButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));
    ui.statisticButton->setIcon(QIcon(":/icons/icon_statistic.png"));
    ui.calendar->setIcon(QIcon(":/icons/icon_calendar.png"));
    ui.planButton->setIcon(QIcon(":/icons/icon_plan.png"));
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
    connect(ui.planButton, &QPushButton::clicked, this, [&] { MainPresenter::get().newTreatmentPlan(); });
    connect(ui.donateButton, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://dinodent.bg/donate/", QUrl::TolerantMode)); });
    connect(ui.ircButton, &QPushButton::clicked, this, [&] { 

        QMainWindow* mainWin = qobject_cast<QMainWindow*>(QApplication::activeWindow());

        if (mainWin) {
            static_cast<DinoDent*>(mainWin)->openIrc();
        } 
    });

    //loading tips

    QFile file(":/other/tips.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);

    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd())
    {
        const QString line = in.readLine().trimmed();

        if (line.isEmpty()) continue;

        m_tips.append(line);
    }

    ui.tipFrame->setFrameColor(Theme::border);

    Theme::applyLightShadow(ui.tipFrame);
}

void WelcomeWidget::refreshTip()
{
    if (m_tips.isEmpty())
        return;

    ui.titleLabel->setText("Знаете ли, че... ");
    ui.titleLabel->setStyleSheet("color: " + Theme::colorToString(Theme::fontTurquoise));
	ui.tipLabel->show();

    auto currentHour = QTime::currentTime().hour();
    
    int idx = QRandomGenerator::global()->bounded(m_tips.size());
    ui.tipLabel->setText(m_tips.at(idx));

    //checking for dynamic message

    auto reply = NetworkManager::simpleRequest(
        "https://www.dinodent.bg/msg"
    );

    QObject::connect(reply, &QNetworkReply::finished, this, [=] {

        QString msg = reply->readAll();

        if (msg.trimmed().isEmpty() || reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            return;
        }

        int n = msg.indexOf('\n');

        QString title;
        QString body;

        if (n < 0) {
            title = msg.trimmed();
            body = QString();
        }
        else {
            title = msg.left(n).trimmed();
            int start = n + 1;
            if (n > 0 && msg[n - 1] == '\r')
                title = msg.left(n - 1).trimmed();

            body = msg.mid(start).trimmed();
        }

        ui.titleLabel->setStyleSheet("color: " + Theme::colorToString(Theme::fontRed));

        if (body.isEmpty()) {
            ui.tipLabel->hide();
        }

        ui.titleLabel->setText(title);
        ui.tipLabel->setText(body);
        });
}

WelcomeWidget::~WelcomeWidget()
{}
