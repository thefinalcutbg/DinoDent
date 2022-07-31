#include "Torque.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QPixmap>
#include <QFileDialog>
#include <QFontDatabase>
#include "View/Theme.h"
#include "View/SettingsDialog/SettingsDialog.h"



QColor blue(133, 207, 234);

Torque::Torque(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    showMaximized();

    ui.hisButton->hide();

    ui.newButton->setIcon(QIcon(":/icons/icon_sheet.png"));
    ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
    ui.saveButton->setIcon(QIcon(":/icons/icon_save.png"));
    ui.saveAsButton->setIcon(QIcon(":/icons/icon_saveAs.png"));
    ui.listSelectButton->setIcon(QIcon(":/icons/icon_open.png"));
    ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
    ui.pisButton->setIcon(QIcon(":/icons/icon_nzok.png"));
    ui.settingsButton->setIcon(QIcon(":/icons/icon_settings.png"));

    QAction* settingsAction = new QAction(u8"Настройки");
    QAction* exitAction = new QAction(u8"Изход");
    QMenu* userMenu = new QMenu();
    userMenu->addAction(settingsAction);
    userMenu->addAction(exitAction);

    connect(ui.newButton, &QPushButton::clicked, [&] { presenter.newAmbPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [&] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { presenter.save(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] {presenter.showListSelector(); });
    connect(ui.printButton, &QPushButton::clicked, [&] {presenter.printPressed(); });
    connect(ui.perioButton, &QPushButton::clicked, [&] {presenter.newPerioPressed(); });
    connect(settingsAction, &QAction::triggered, [&] {presenter.userSettingsPressed();});
    connect(ui.pisButton, &QPushButton::clicked, [&] { presenter.pisDialog();});
    connect(ui.settingsButton, &QPushButton::clicked, [&] { presenter.settingsPressed();});
    connect(ui.hisButton, &QPushButton::clicked, [&] { test.sendRequest(11);});

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

ITabView* Torque::tabView()
{
    return ui.tabView;
}

void Torque::setUserLabel(const std::string& doctorName, const std::string& practiceName)
{
    ui.userButton->setText(QString::fromStdString("  " + doctorName));
    ui.practiceLabel->setText(QString::fromStdString(practiceName));
}

void Torque::exitProgram()
{
    QApplication::quit();
}

bool Torque::initialized()
{
    return m_initialized;
}

void Torque::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(rect(), Theme::mainBackgroundColor);
    painter.end();
}

void Torque::closeEvent(QCloseEvent* event)
{
    if (!presenter.closeAllTabs())
        event->ignore();

    foreach(QWidget * widget, QApplication::topLevelWidgets()) 
    {
        if (widget == this) continue;
        widget->close();
    }
}
