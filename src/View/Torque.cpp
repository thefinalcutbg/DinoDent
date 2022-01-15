#include "Torque.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QPixmap>
QColor blue(133, 207, 234);

Torque::Torque(QWidget* parent)
    : QMainWindow(parent)
{

    ui.setupUi(this);
    showMaximized();

    QAction* settingsAction = new QAction("Настройки");
    QAction* exitAction = new QAction("Изход");
    QMenu* userMenu = new QMenu();
    userMenu->addAction(settingsAction);
    userMenu->addAction(exitAction);

    connect(ui.newButton, &QPushButton::clicked, [&] { presenter.newAmbPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [&] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { presenter.save(); });
    connect(ui.tabView, &TabView::closeRequested, [&] {presenter.closeTab(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] {presenter.showListSelector(); });
    connect(ui.printButton, &QPushButton::pressed, [&] {presenter.printPressed(); });
    connect(ui.perioButton, &QPushButton::pressed, [&] {presenter.newPerioPressed(); });




    connect(exitAction, &QAction::triggered, [&] { presenter.logOut(); });


    ui.userButton->setMenu(userMenu);
    ui.userButton->setPopupMode(QToolButton::InstantPopup);
    ui.userButton->setIconSize(QSize(25, 25));
    ui.userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    ui.userButton->setIcon(QIcon("user.png"));

    presenter.setView(this);
}

ITabView* Torque::tabView()
{
    return ui.tabView;
}

void Torque::setDoctor(const std::string& name)
{
    ui.userButton->setText(QString::fromStdString(name));
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
    painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
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
