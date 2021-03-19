#include "Torque.h"

QColor blue(0, 122, 204);

Torque::Torque(QWidget* parent)
    : QMainWindow(parent),
    ambListPage(&database, this)
{

    ui.setupUi(this);
    ui.stackedWidget->insertWidget(0, &ambListPage);


    showMaximized();
}

void Torque::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
    painter.fillRect(QRect(0, 0, 150, height()), blue);
    painter.end();
}

void Torque::closeEvent(QCloseEvent* event)
{
    if (!ambListPage.closeAllTabs())
        event->ignore();
}
