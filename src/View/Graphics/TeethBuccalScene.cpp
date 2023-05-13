#include "TeethBuccalScene.h"
#include "View/Graphics/ToothGraphicsItem.h"
#include "View/Graphics/ToothPainter.h"
#include "View/Graphics/PerioChartItem.h"
#include "View/Graphics/SelectionBox.h"
#include <QGraphicsSceneMouseEvent>


TeethBuccalScene::TeethBuccalScene()
{
    int posY = 15;
    int posX = 15;

    int selectionBox_posY = posY;

    for (int i = 0; i < 32; i++)
    {
        if (i == 16)
        {
            posY += 225;
            selectionBox_posY += 249;
        }

        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setZValue(1);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        selectionBox[i] = new SelectionBox(i);
        selectionBox[i]->setZValue(3);
        selectionBox[i]->setPos(posX, selectionBox_posY);
        addItem(selectionBox[i]);

        dsnToothGraphic[i] = new ToothGraphicsItem(i);
        dsnToothGraphic[i]->setZValue(0);

        int dnsPos = (i < 8 || i > 23) ?
            posX - (dsnToothGraphic[i]->boundingRect().width() / 2)
            :
            posX + (dsnToothGraphic[i]->boundingRect().width() / 2);

        dsnToothGraphic[i]->setPos(dnsPos, posY);
        addItem(dsnToothGraphic[i]);
        

        if (i < 15)
            posX += toothGraphic[i]->boundingRect().width();
        else if (i > 15)
            posX -= toothGraphic[i]->boundingRect().width();

        if (i == 18) posX += 18;
        else if (i == 28) posX -= 18;
    }

    maxillaryChart = new PerioChartItem(false);
    maxillaryChart->setPos(15, 15);
    maxillaryChart->setZValue(2);
    addItem(maxillaryChart);

    mandibularChart = new PerioChartItem(false);
    //mandibularChart->setTransform(QTransform::fromScale(-1, 1));
    mandibularChart->setZValue(2);
    mandibularChart->setRotation(180);
    mandibularChart->setPos(mandibularChart->boundingRect().width() + 15, 460);
    addItem(mandibularChart);
}


void TeethBuccalScene::display(const ToothPaintHint& tooth)
{
    toothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalOcclusal(tooth));

    if (tooth.dsn)
    {
        dsnToothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalOcclusal(*tooth.dsn));
    }
    else
    {
        dsnToothGraphic[tooth.idx]->drawEmpty();
    }
}

void TeethBuccalScene::setProcedures(std::vector<int> teeth_procedures)
{
    for (auto& t : toothGraphic)
        t->setProcedure(false);

    for (auto& p : teeth_procedures)
        if (p != -1) toothGraphic[p]->setProcedure(true);
}

void TeethBuccalScene::setMeasurments(const int pd[192], const int cal[192])
{
    for (int i = 0; i < 48; i+=3) {

        int maxIdx = i * 2;
        int mandIdx = 96 + maxIdx + 3;

        maxillaryChart->setMeasurment(i, pd[maxIdx] - cal[maxIdx], cal[maxIdx], false);
        maxillaryChart->setMeasurment(i+1, pd[maxIdx + 1] - cal[maxIdx + 1], cal[maxIdx + 1], false);
        maxillaryChart->setMeasurment(i+2, pd[maxIdx + 2] - cal[maxIdx + 2], cal[maxIdx + 2], false);

        mandibularChart->setMeasurment(i, pd[mandIdx] - cal[mandIdx], cal[mandIdx], false);
        mandibularChart->setMeasurment(i+1, pd[mandIdx + 1] - cal[mandIdx + 1], cal[mandIdx + 1], false);
        mandibularChart->setMeasurment(i+2, pd[mandIdx + 2] - cal[mandIdx + 2], cal[mandIdx + 2], false);
    }

    maxillaryChart->update();
    mandibularChart->update();

}

void TeethBuccalScene::showPerio(bool shown)
{
    if (shown) {
        maxillaryChart->show();
        mandibularChart->show();
    }
    else {
        maxillaryChart->hide();
        mandibularChart->hide();
    }

}

int TeethBuccalScene::selectedTooth()
{
    if (selectedItems().empty()) return -1;

    return static_cast<SelectionBox*>(selectedItems()[0])->getIndex();
}

void TeethBuccalScene::setSelectedTooth(int toothIdx)
{
    
    QSignalBlocker b(this);

    for (auto i : selectedItems()) i->setSelected(false);

    if (toothIdx == -1) return;

    selectionBox[toothIdx]->setSelected(true);
  
}

void TeethBuccalScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //disables multiselection
    if (event->modifiers() == Qt::Modifier::CTRL) {
        event->setModifiers(Qt::KeyboardModifier::NoModifier);

    }

    QGraphicsScene::mousePressEvent(event);
}
