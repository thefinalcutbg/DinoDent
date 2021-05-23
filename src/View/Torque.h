#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>

#include "Database/Database.h"
#include "ui_Torque.h"
#include "AmbListPage/AmbListPage.h"

class Torque : public QMainWindow
{
    Q_OBJECT

    AmbListPage ambListPage;


    void paintEvent(QPaintEvent* event);
    void closeEvent(QCloseEvent* event);

public:
    Torque(QWidget *parent = Q_NULLPTR);

private:
    Ui::TorqueClass ui;
};
