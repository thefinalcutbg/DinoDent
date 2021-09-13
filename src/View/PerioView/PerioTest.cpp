#include "PerioTest.h"
#include "View/ListView/TeethView/TeethViewScene.h"
#include <QPainter>
#include <QDebug>
PerioTest::PerioTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.perioData_2->setLingual(true);

    connect(ui.toothHeader, &PerioSurfaceHeader::toothStateChanged,
        [&](int tooth, bool present)
        {
            ui.perioData->setExtracted(tooth, !present);
            ui.perioData_2->setExtracted(tooth, !present);
        });
    
    perioScene = new PerioScene();

    upperBuccal = new PerioChartItem();
    perioScene->addItem(upperBuccal);
    upperBuccal->setPos(0, -180);

    upperLingual = new PerioChartItem();
    perioScene->addItem(upperLingual);
    upperLingual->setPos(0, 0);

    ui.graphicsView->setScene(perioScene);

    connect(ui.perioData, &PerioSurfaceData::measuremetChanged,
        [&](Measurment m) {
            upperBuccal->setMeasurment(m);
        });

}

void PerioTest::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::GlobalColor::white);
}