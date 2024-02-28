#include "PerioScene.h"

#include "View/Graphics/ToothGraphicsItem.h"
#include "View/Graphics/ToothPainter.h"
#include "View/Graphics/PaintHint.h"

PerioScene::PerioScene()
{
   

    double posY = 0;
    double posX = 0;

    //double centerX = 32;

    for (int i = 0; i < 16; i++)
    {
        
        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setWidth(70);
        //posX = centerX - (toothGraphic[i]->boundingRect().width() / 2);
        toothGraphic[i]->showLingual(true);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        dsnToothGraphic[i] = new ToothGraphicsItem(i);
        dsnToothGraphic[i]->setWidth(70);
        dsnToothGraphic[i]->showLingual(true);
        dsnToothGraphic[i]->setZValue(-1);

        int dnsPos = (i < 8) ?
            posX - (dsnToothGraphic[i]->boundingRect().width() / 2)
            :
            posX + (dsnToothGraphic[i]->boundingRect().width() / 2);

        dsnToothGraphic[i]->setPos(dnsPos, posY);
        addItem(dsnToothGraphic[i]);

        //centerX = centerX + 70;

        posX += toothGraphic[i]->boundingRect().width();
    }

}

void PerioScene::display(const ToothPaintHint& tooth)
{
    if (tooth.idx < 16) {

        toothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(tooth));
        
        tooth.dsn ?
            dsnToothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(*tooth.dsn.get()))
            :
            dsnToothGraphic[tooth.idx]->drawEmpty();
    }

    else {

        toothGraphic[31 - tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(tooth));
        
        tooth.dsn ?
            dsnToothGraphic[31 - tooth.idx]->setToothPixmap(ToothPainter::getBuccalLingual(*tooth.dsn.get()))
            :
            dsnToothGraphic[31 - tooth.idx]->drawEmpty();
    }

}

void PerioScene::deletePixmaps()
{
    for (auto &t : toothGraphic)
        t->setToothPixmap(QPixmap());
}

PerioScene::~PerioScene()
{
}
