#include "PerioScene.h"

#include "View/PerioView/Widgets/Measurment.h"
#include "View/ListView/TeethView/ToothGraphicsItem.h"
#include "View/ListView/ToothPaintDevices/ToothPainter.h"

#include "Model/Tooth/ToothContainer.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"

PerioScene::PerioScene()
{
    bool molar;

    int posY = 0;
    int posX = 0;

    for (int i = 0; i < 32; i++)
    {
        if (i > 2 && i < 13 || i > 18 && i < 29) molar = false;
        else molar = true;

        if (i == 16)
        {
            posY += 225;
        }

        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        if (i < 15)
            posX += toothGraphic[i]->boundingRect().width();
        else if (i > 15)
            posX -= toothGraphic[i]->boundingRect().width();

        if (i == 18) posX += 18;
        else if (i == 28) posX -= 18;
    }



    ToothContainer teeth;
    
    auto hints = ToothHintCreator::getTeethHint(teeth);

    for (auto& h : hints)
        display(h);
    
}

void PerioScene::display(const ToothPaintHint& tooth)
{
    
    ToothPainter toothPainter;
    toothGraphic[tooth.idx]->setToothGraphic(toothPainter.paintTooth(tooth));
}


PerioScene::~PerioScene()
{
}
