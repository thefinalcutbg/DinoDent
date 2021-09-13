#pragma once

#include <QWidget>
#include "ui_PerioTest.h"
#include "PerioView/PerioChartItem.h"

class PerioTest : public QWidget
{
    Q_OBJECT

    PerioScene* perioScene;
    PerioChartItem* upperBuccal;
    PerioChartItem* upperLingual;

    void paintEvent(QPaintEvent* event) override;

public:
    PerioTest(QWidget *parent = Q_NULLPTR);
    

private:
    Ui::PerioTestClass ui;
};
