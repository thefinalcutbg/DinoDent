#include "TeethLingualScene.h"
#include "TeethBuccalScene.h"
#include "View/ListView/TeethView/ToothGraphicsItem.h"
#include "View/ToothPaintDevices/ToothPainter.h"
#include "View/PerioView/PerioGraphics/PerioChartItem.h"
#include "View/ListView/TeethView/DsnToothGraphicsItem.h"

TeethLingualScene::TeethLingualScene()
{
    int posY = 15;
    int posX = 15;

    for (int i = 0; i < 32; i++)
    {
        if (i == 16)
        {
            posY += 225;
        }

        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        dsnToothGraphic[i] = new DsnToothGraphicsItem(i);
        dsnToothGraphic[i]->setZValue(-1);

        int dnsPos = (i < 8 || i > 23) ?
            posX + (dsnToothGraphic[i]->boundingRect().width() / 2)
            :
            posX - (dsnToothGraphic[i]->boundingRect().width() / 2);

        dsnToothGraphic[i]->setPos(dnsPos, posY);
        addItem(dsnToothGraphic[i]);


        if (i < 15)
            posX -= toothGraphic[i]->boundingRect().width();
        else if (i > 15)
            posX += toothGraphic[i]->boundingRect().width();

        if (i == 2) posX += 18;
        else if (i == 12) posX -= 18;
    }

    maxillaryChart = new PerioChartItem(false);
    maxillaryChart->setTransform(QTransform::fromScale(-1, 1));
    maxillaryChart->setPos(68, 12);
    addItem(maxillaryChart);

    mandibularChart = new PerioChartItem(false);
    mandibularChart->setTransform(QTransform::fromScale(-1, 1));
    mandibularChart->setRotation(180);
    mandibularChart->setPos(68-mandibularChart->boundingRect().width(), 468);
    addItem(mandibularChart);
}

void TeethLingualScene::display(const ToothPaintHint& tooth)
{
    toothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getLingualOcclusal(tooth));
    dsnToothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getDnsLingual(tooth));
}

void TeethLingualScene::setProcedures(std::vector<int> teeth_procedures)
{
    for (auto& t : toothGraphic)
        t->setProcedure(false);

    for (auto& p : teeth_procedures)
        if (p != -1) toothGraphic[p]->setProcedure(true);
}

void TeethLingualScene::setMeasurments(const int pd[192], const int cal[192])
{
    for (int i = 0; i < 48; i += 3) {

        int maxIdx = i * 2;
        maxIdx += 3;

        int mandIdx = 96 + maxIdx - 3 - 1; //why -1 ?



        maxillaryChart->setMeasurment(i, pd[maxIdx] - cal[maxIdx], cal[maxIdx], false);
        maxillaryChart->setMeasurment(i + 1, pd[maxIdx + 1] - cal[maxIdx + 1], cal[maxIdx + 1], false);
        maxillaryChart->setMeasurment(i + 2, pd[maxIdx + 2] - cal[maxIdx + 2], cal[maxIdx + 2], false);

        mandibularChart->setMeasurment(i, pd[mandIdx] - cal[mandIdx], cal[mandIdx], false);
        mandibularChart->setMeasurment(i + 1, pd[mandIdx + 1] - cal[mandIdx + 1], cal[mandIdx + 1], false);
        mandibularChart->setMeasurment(i + 2, pd[mandIdx + 2] - cal[mandIdx + 2], cal[mandIdx + 2], false);
    }

    maxillaryChart->update();
    mandibularChart->update();
}

void TeethLingualScene::showPerio(bool shown)
{
    if (shown) {
        maxillaryChart->show();
        mandibularChart->show();
    }
    else{
        maxillaryChart->hide();
        mandibularChart->hide();
    }

}

