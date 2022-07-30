#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>

#include "Database/Database.h"
#include "Presenter/MainPresenter.h"
#include "ui_Torque.h"

#include "IMainView.h"
#include "Network/HIS/HisService.h"

class Torque : public QMainWindow, public IMainView
{
    Q_OBJECT

    MainPresenter presenter;

    void paintEvent(QPaintEvent* event);
    void closeEvent(QCloseEvent* event);

public:
    Torque(QWidget *parent = Q_NULLPTR);
    ITabView* tabView() override;
    void setUserLabel(const std::string& doctorName, const std::string& practiceName) override;
    void exitProgram() override;
    bool initialized();

private:
    Ui::TorqueClass ui;
};
