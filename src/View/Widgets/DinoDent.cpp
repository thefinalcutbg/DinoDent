#include "DinoDent.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QPixmap>
#include <QFileDialog>
#include <QFontDatabase>
#include "View/Theme.h"
#include "View/Widgets/SettingsDialog.h"

QColor blue(133, 207, 234);

DinoDent::DinoDent(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
   // showMaximized();

    ui.newButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.perscrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
    ui.saveButton->setIcon(QIcon(":/icons/icon_save.png"));
    ui.listSelectButton->setIcon(QIcon(":/icons/icon_open.png"));
    ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
    ui.pisButton->setIcon(QIcon(":/icons/icon_nzok.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));


    QAction* settingsAction = new QAction("Настройки");
    settingsAction->setIcon(QIcon(":/icons/icon_settings.png"));
    QAction* exitAction = new QAction("Изход");
    exitAction->setIcon(QIcon(":/icons/icon_remove.png"));
    QMenu* userMenu = new QMenu(this);
    userMenu->addAction(settingsAction);
    userMenu->addAction(exitAction);
    userMenu->setStyleSheet(Theme::getPopupMenuStylesheet());

    connect(ui.newButton, &QPushButton::clicked, [&] { presenter.newAmbPressed(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { presenter.save(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] {presenter.showListSelector(); });
    connect(ui.perscrButton, &QPushButton::clicked, [&] { presenter.newPerscriptionPressed(); });
    connect(ui.printButton, &QPushButton::clicked, [&] {presenter.printPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] {presenter.newPerioPressed(); });
    connect(settingsAction, &QAction::triggered, [&] {presenter.userSettingsPressed();});
    connect(ui.pisButton, &QPushButton::clicked, [&] { presenter.pisDialog();});
    connect(ui.settingsButton, &QPushButton::clicked, [&] { presenter.settingsPressed();});
    
    connect(exitAction, &QAction::triggered, [&] { presenter.logOut(); });

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
/*
    setStyleSheet(
        "QMenu{"
        " background-color: white;"
        " }"

        "   QMenu::item {"
        "   padding: 2px 20px 2px 20px;"
        "  border: 1px solid transparent; "// reserve space for selection border
        "   spacing: 20px;"
        "    }"


        "  QMenu::item:selected {"
        "       border-color: darkblue;"
        "        background: rgb(53, 120, 191, 150);"
        "       color:white;"
        "      }"

        "    QMenu::separator {"
        "       height: 2px;"
        "         margin: 2px 5px 2px 4px;"
        "      }"

        "     QMenu::indicator {"
        "       width: 20px;"
        "       height: 13px;"
        "    }"
    );
 */
    presenter.setView(this);
}

ITabView* DinoDent::tabView()
{
    return ui.tabView;
}

void DinoDent::setUserLabel(const std::string& doctorName, const std::string& practiceName)
{
    ui.userButton->setText(QString::fromStdString("  " + doctorName));
    ui.practiceLabel->setText(QString::fromStdString(practiceName));
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
    painter.end();
}

void DinoDent::closeEvent(QCloseEvent* event)
{
    if (!presenter.closeAllTabs())
        event->ignore();

    foreach(QWidget * widget, QApplication::topLevelWidgets()) 
    {
        if (widget == this) continue;
        widget->close();
    }
}