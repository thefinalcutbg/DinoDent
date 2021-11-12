#include "Torque.h"

QColor blue(133, 207, 234);

Torque::Torque(QWidget* parent)
    : QMainWindow(parent)
{

    ui.setupUi(this);
    showMaximized();

    connect(ui.newButton, &QPushButton::clicked, [&] { presenter.newAmbPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [&] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { presenter.save(); });
    connect(ui.tabView, &TabView::closeRequested, [&] {presenter.closeTab(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] {presenter.showListSelector(); });
    connect(ui.printButton, &QPushButton::pressed, [&] {presenter.printPressed(); });
    connect(ui.perioButton, &QPushButton::pressed, [&] {presenter.newPerioPressed(); });
    presenter.setView(this);
}

ITabView* Torque::tabView()
{
    return ui.tabView;
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
