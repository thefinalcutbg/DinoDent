#include "PerioScene.h"

#include "View/PerioView/Widgets/Measurment.h"
#include "View/ListView/TeethView/ToothGraphicsItem.h"
#include "Model/Tooth/ToothContainer.h"
#include <QDebug>
PerioScene::PerioScene()
{

    toothPainter.showLingual = true;

    double posY = 0;
    double posX = 0;

    double centerX = 32;

    for (int i = 0; i < 16; i++)
    {
        
        toothGraphic[i] = new ToothGraphicsItem(i);
        posX = centerX - (toothGraphic[i]->boundingRect().width() / 2);
        toothGraphic[i]->showLingual(true);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        centerX = centerX + 70;



        //posX += toothGraphic[i]->boundingRect().width();
    }

}

void PerioScene::display(const ToothPaintHint& tooth)
{
    if(tooth.idx < 16)
    toothGraphic[tooth.idx]->setToothGraphic(toothPainter.paintTooth(tooth));
    else
    toothGraphic[31-tooth.idx]->setToothGraphic(toothPainter.paintTooth(tooth));
}

PerioScene::~PerioScene()
{
}
