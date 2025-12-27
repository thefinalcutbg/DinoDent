#include "DinoDent.h"

#include <QAction>
#include <QMenu>

#include <QPixmap>
#include <QFileDialog>
#include <QFontDatabase>
#include <QShortcut>
#include <QStatusBar>
#include <QTimer>
#include <QDesktopServices>

#include "Presenter/MainPresenter.h"

#include "View/Theme.h"
#include "View/Widgets/GlobalWidgets.h"
#include "View/Widgets/AboutDialog.h"
#include "View/Widgets/SplashScreen.h"
#include "View/Widgets/NotificationListDialog.h"

#include "Model/User.h"
#include "Model/User.h"

#include "Database/DbNotification.h"

#include "ChatDialog.h"
#include "src/Version.h"

#ifdef Q_OS_WIN
#include <QWindow>
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

DinoDent::DinoDent(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.statusBar->hide();

    setWindowState(Qt::WindowMaximized);

#ifdef Q_OS_WIN
/*
    auto& tbClr = Theme::mainBackgroundColor;
    auto tbTxt = QColor(Qt::black);

    const COLORREF rgb = RGB(tbClr.red(), tbClr.green(), tbClr.blue());

    const DWORD dwmCaptionAttr = 35;
    const DWORD dwmTextAttr = 36;

	auto hwnd = reinterpret_cast<HWND>(windowHandle()->winId());

    DwmSetWindowAttribute(hwnd, dwmCaptionAttr, &rgb, sizeof(rgb));

    const COLORREF white = RGB(tbTxt.red(), tbTxt.green(), tbTxt.blue());
    DwmSetWindowAttribute(hwnd, dwmTextAttr, &white, sizeof(white));
 */
#endif

    m_chatDialog = new ChatDialog(this);

    GlobalWidgets::mainWindow = this;
    GlobalWidgets::statusBar = statusBar();

    statusBar()->setStyleSheet("font-weight: bold; color:" + Theme::colorToString(Theme::fontTurquoiseClicked));

    QAction* settingsAction = new QAction("Настройки");
    settingsAction->setIcon(QIcon(":/icons/icon_settings.png"));
    QAction* exitAction = new QAction("Изход");
    exitAction->setIcon(QIcon(":/icons/icon_remove.png"));
    QMenu* userMenu = new QMenu(this);
    userMenu->addAction(settingsAction);
    userMenu->addAction(exitAction);
    userMenu->setStyleSheet(Theme::getPopupMenuStylesheet());

    //setting global shortcuts
    auto shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().save(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_P), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().printPressed(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_O), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().showBrowser(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().newAmbPressed(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_M), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().newPerioPressed(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().newInvoicePressed(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().newPrescriptionPressed(); });

    shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(shortcut, &QShortcut::activated, [&] { MainPresenter::get().pdfPressed(); });

    //setting buttons
    ui.newButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.perscrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
    ui.saveButton->setIcon(QIcon(":/icons/icon_save.png"));
    ui.browserButton->setIcon(QIcon(":/icons/icon_open.png"));
    ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
    ui.pisButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));
    ui.planButton->setIcon(QIcon(":/icons/icon_plan.png"));
    ui.statisticButton->setIcon(QIcon(":/icons/icon_statistic.png"));
    ui.calendarButton->setIcon(QIcon(":/icons/icon_calendar.png"));
    ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
    ui.aboutButton->setIcon(QIcon(":/icons/icon_question.png"));
    ui.mircButton->setIcon(QIcon(":/icons/icon_mirc.png"));
    ui.notifButton->setIcon(QIcon(":/icons/icon_bell.png"));
    ui.pdfButton->setIcon(QIcon(":/icons/icon_pdf.png"));
    ui.donateButton->setIcon(QIcon(":/icons/icon_donate.png"));
    ui.notifButton->setMonochrome(true);
    ui.mircButton->setMonochrome(true);
    
    connect(ui.donateButton, &QPushButton::clicked, [&] { QDesktopServices::openUrl(QUrl("https://dinodent.bg/donate/", QUrl::TolerantMode)); });
    connect(ui.newButton, &QPushButton::clicked, [&] { MainPresenter::get().newAmbPressed(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { MainPresenter::get().save(); });
    connect(ui.browserButton, &QPushButton::clicked, [&] { MainPresenter::get().showBrowser(); });
    connect(ui.perscrButton, &QPushButton::clicked, [&] { MainPresenter::get().newPrescriptionPressed(); });
    connect(ui.printButton, &QPushButton::clicked, [&] { MainPresenter::get().printPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] { MainPresenter::get().newPerioPressed(); });
    connect(ui.calendarButton, &QPushButton::clicked, [&] { MainPresenter::get().openCalendar(); });
    connect(ui.statisticButton, &QPushButton::clicked, [&] { MainPresenter::get().statisticPressed(); });
    connect(ui.planButton, &QPushButton::clicked, [&] { MainPresenter::get().newTreatmentPlan(); });
    connect(settingsAction, &QAction::triggered, [&] { MainPresenter::get().userSettingsPressed();});
    connect(ui.pisButton, &QPushButton::clicked, [&] { MainPresenter::get().pisDialog();});
    connect(ui.settingsButton, &QPushButton::clicked, [&] { MainPresenter::get().settingsPressed();});
    connect(ui.invoiceButton, &QPushButton::clicked, [&] { MainPresenter::get().newInvoicePressed(); });
    connect(ui.pdfButton, &QPushButton::clicked, [&] { MainPresenter::get().pdfPressed(); });
    connect(ui.aboutButton, &QPushButton::clicked, this, [&] { AboutDialog d; d.exec(); });
    connect(ui.mircButton, &QPushButton::clicked, this, [&] { 
    connect(ui.userButton, &QPushButton::clicked, this, [&] { ui.userButton->showMenu(); });

        if (!m_chatDialog) return;

        if (!User::isValid()) {
            ModalDialogBuilder::showMessage(
                "За да използвате тази функция въведете коректни данни от Настройки!"
            );

            return;
        }

        setIrcIcon(false);
        
        m_chatDialog->openAndConnect();
        
    });

    connect(exitAction, &QAction::triggered, [&] { MainPresenter::get().logOut(); });

    connect(ui.notifButton, &QPushButton::clicked, this, [&]{ MainPresenter::get().notificationPressed();});

    ui.userButton->setMenu(userMenu);
    ui.userButton->setIconSize(QSize(25, 25));
   
    ui.userButton->setIcon(QIcon{":/icons/icon_user.png"});
/*
    ui.practiceLabel->setStyleSheet("color:" + Theme::colorToString(Theme::practiceLabel));
    
    QFont font;
    font.setBold(true);
    ui.practiceLabel->setFont(font);
*/
    SplashScreen::hideAndDestroy();

    ui.tabView->showWelcomeScreen();

    MainPresenter::get().setView(this);

}

TabView* DinoDent::tabView()
{
    return ui.tabView;
}

void DinoDent::setUserLabel(const std::string& doctorName, const std::string& practiceName)
{
    ui.userButton->setText(QString::fromStdString(doctorName));
    //ui.practiceLabel->setText(QString::fromStdString(practiceName));

    QString title = "DinoDent v";
    title += Version::current().toString().c_str();
    title += " ";
    title += practiceName.c_str();

    setWindowTitle(title);
}

void DinoDent::exitProgram()
{
    QApplication::quit();
}

bool DinoDent::initialized()
{
    return m_loggedIn;
}

void DinoDent::disableButtons(bool printDisabled, bool saveDisabled, bool pdfDisabled)
{
    ui.printButton->setDisabled(printDisabled);
    ui.saveButton->setDisabled(saveDisabled);
    ui.pdfButton->setDisabled(pdfDisabled);
}

void DinoDent::setIrcIcon(bool glow)
{
    ui.mircButton->setMonochrome(!glow);
}

void DinoDent::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(rect(), Theme::mainBackgroundColor);
    painter.fillRect(0, height() - 21, width(), 21, QColor(240, 240, 240));
    painter.end();
}

void DinoDent::closeEvent(QCloseEvent* event)
{
    if (!MainPresenter::get().closeAllTabs())
        event->ignore();

    foreach(QWidget * widget, QApplication::topLevelWidgets()) 
    {
        if (widget == this) continue;
        widget->close();
    }
}

void DinoDent::setNotificationIcon(int activeNotifCount)
{
    ui.notifButton->setMonochrome(!activeNotifCount);
    ui.notifButton->setIcon(activeNotifCount ? QIcon(":/icons/icon_bell_notify.png") : QIcon(":/icons/icon_bell.png"));

    switch(activeNotifCount){
        case 0: ui.notifButton->setToolTip("Няма активни напомняния"); break;
        case 1: ui.notifButton->setToolTip("1 активно напомняне"); break;
        default: ui.notifButton->setToolTip(QString::number(activeNotifCount) + " активни напомняния");
    }
}

void DinoDent::openIrc()
{
    ui.mircButton->click();
}

DinoDent::~DinoDent()
{
    Theme::cleanUpFusionStyle();
}
