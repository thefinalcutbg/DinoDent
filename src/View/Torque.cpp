#include "Torque.h"

QColor blue(133, 207, 234);

Torque::Torque(QWidget* parent)
    : QMainWindow(parent),
    view(this)
{

    ui.setupUi(this);
    ui.stackedWidget->insertWidget(0, &view);
    showMaximized();
}

void Torque::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
   // painter.fillRect(QRect(0, 0, width(), height()), blue);
    painter.end();
}

void Torque::closeEvent(QCloseEvent* event)
{
    if (!view.closeAllTabs())
        event->ignore();

    foreach(QWidget * widget, QApplication::topLevelWidgets()) 
    {
        if (widget == this) continue;
        widget->close();
    }
}
