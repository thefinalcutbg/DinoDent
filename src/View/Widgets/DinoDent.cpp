#include "DinoDent.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QPixmap>
#include <QFileDialog>
#include <QFontDatabase>
#include "View/Theme.h"
#include "View/Widgets/SettingsDialog.h"
#include <QStatusBar>
#include "View/Widgets/GlobalWidgets.h"
#include "View/Widgets/AboutDialog.h"
#include "Version.h"

QColor blue(133, 207, 234);

DinoDent::DinoDent(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.newButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.perscrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
    ui.saveButton->setIcon(QIcon(":/icons/icon_save.png"));
    ui.listSelectButton->setIcon(QIcon(":/icons/icon_open.png"));
    ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
    ui.pisButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));
    ui.statisticButton->setIcon(QIcon(":/icons/icon_statistic.png"));
    ui.aboutButton->setIcon(QIcon(":/icons/icon_question.png"));

    GlobalWidgets::statusBar = statusBar();

    statusBar()->setStyleSheet("font-weight: bold; color:" + Theme::colorToString(Theme::fontTurquoiseClicked));
   // statusBar()->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoiseClicked));


    QAction* settingsAction = new QAction("Настройки");
    settingsAction->setIcon(QIcon(":/icons/icon_settings.png"));
    QAction* exitAction = new QAction("Изход");
    exitAction->setIcon(QIcon(":/icons/icon_remove.png"));
    QMenu* userMenu = new QMenu(this);
    userMenu->addAction(settingsAction);
    userMenu->addAction(exitAction);
    userMenu->setStyleSheet(Theme::getPopupMenuStylesheet());

    connect(ui.newButton, &QPushButton::clicked, [&] { MainPresenter::get().newAmbPressed(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { MainPresenter::get().save(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] { MainPresenter::get().showListSelector(); });
    connect(ui.perscrButton, &QPushButton::clicked, [&] { MainPresenter::get().newPerscriptionPressed(); });
    connect(ui.printButton, &QPushButton::clicked, [&] { MainPresenter::get().printPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] { MainPresenter::get().newPerioPressed(); });
    connect(ui.statisticButton, &QPushButton::clicked, [&] { MainPresenter::get().statisticPressed(); });
    connect(settingsAction, &QAction::triggered, [&] { MainPresenter::get().userSettingsPressed();});
    connect(ui.pisButton, &QPushButton::clicked, [&] { MainPresenter::get().pisDialog();});
    connect(ui.settingsButton, &QPushButton::clicked, [&] { MainPresenter::get().settingsPressed();});
    connect(ui.aboutButton, &QPushButton::clicked, [&] { AboutDialog d; d.exec(); });

    connect(exitAction, &QAction::triggered, [&] { MainPresenter::get().logOut(); });

   // ui.statisticButton->hide();

    ui.userButton->setMenu(userMenu);
    ui.userButton->setPopupMode(QToolButton::InstantPopup);
    ui.userButton->setIconSize(QSize(25, 25));
    ui.userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui.userButton->setIcon(QIcon{":/icons/icon_user.png"});

    ui.practiceLabel->setStyleSheet("color:" + Theme::colorToString(Theme::practiceLabel));
    
    int id = QFontDatabase::addApplicationFont(":/fonts/font_RobotoCondensedRegular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(family);
    font.setBold(true);
    font.setPointSize(14);
    ui.practiceLabel->setFont(font);

    MainPresenter::get().setView(this);
}

ITabView* DinoDent::tabView()
{
    return ui.tabView;
}

void DinoDent::setUserLabel(const std::string& doctorName, const std::string& practiceName)
{
    ui.userButton->setText(QString::fromStdString("  " + doctorName));
    ui.practiceLabel->setText(QString::fromStdString(practiceName));

    QString title = "DinoDent v";
    title += Version::current().getAsString().c_str();
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

void DinoDent::paintEvent(QPaintEvent* event)
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
