#include "PerioScene.h"

#include "View/ListView/TeethView/ToothGraphicsItem.h"
#include "Model/Tooth/ToothContainer.h"
#include "View/ToothPaintDevices/ToothPainter.h"
#include "View/ToothPaintDevices/PaintHint.h"
#include <QDebug>
PerioScene::PerioScene()
{
   

    double posY = 0;
    double posX = 0;

    double centerX = 32;

    for (int i = 0; i < 16; i++)
    {
        
        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setWidth(70);
        //posX = centerX - (toothGraphic[i]->boundingRect().width() / 2);
        toothGraphic[i]->showLingual(true);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);


        //centerX = centerX + 70;

        posX += toothGraphic[i]->boundingRect().width();
    }

}

void PerioScene::display(const ToothPaintHint& tooth)
{
    if(tooth.idx < 16)
    toothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(tooth));
    else
    toothGraphic[31-tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(tooth));
}

void PerioScene::deletePixmaps()
{
    for (auto &t : toothGraphic)
        t->setToothPixmap(QPixmap());
}

PerioScene::~PerioScene()
{
}