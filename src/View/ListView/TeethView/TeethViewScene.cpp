#include "TeethViewScene.h"
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include "ContextMenu.h"
#include "ToothGraphicsItem.h"
#include "SelectionBox.h"
#include "BridgeItem.h"
#include "Presenter/ListPresenter/ListPresenter.h"

TeethViewScene::TeethViewScene(QObject *parent)
    : QGraphicsScene(parent), contextMenu(nullptr)
{

    //background color:
    {
        QPixmap white(730, 456);
        white.fill(Qt::white);
        QGraphicsPixmapItem* background = new QGraphicsPixmapItem(white);
        addItem(background);
        background->setPos(-24, -3);
    }


    bool molar;

    int posY = 0;
    int posX = 0;

    for(int i = 0; i<32; i++)
    {
        if (i > 2 && i < 13 || i > 18 && i < 29) molar = false;
        else molar = true;

        if (i == 16) posY += 223;

        toothGraphic[i] = new ToothGraphicsItem(i);
        toothGraphic[i]->setZValue(0);
        toothGraphic[i]->setPos(posX, posY);
        addItem(toothGraphic[i]);

        selectionBox[i] = new SelectionBox(i);
        selectionBox[i]->setZValue(2);
        selectionBox[i]->setPos(posX, posY + 37);
        addItem(selectionBox[i]);

        if (i < 15)
            posX += toothGraphic[i]->boundingRect().width();
        else if(i > 15)
            posX -= toothGraphic[i]->boundingRect().width();

        if (i == 18) posX += 18;
        else if (i == 28) posX -= 18;
    }

    upperBridge = new BridgeItem();
    upperBridge->setZValue(1);
    addItem(upperBridge);
    upperBridge->setPos(0, 100);

    lowerBridge = new BridgeItem();
    lowerBridge->setZValue(1);
    addItem(lowerBridge);
    lowerBridge->setTransform(QTransform::fromScale(-1, 1));
    lowerBridge->setPos(684, 295);

    connect(this, &QGraphicsScene::selectionChanged,
        [=]
        {
            std::vector<int> selectedIndexes;
            selectedIndexes.reserve(32);

            SelectionBox* selection;

            for (QGraphicsItem* item : selectedItems())
            {
                selection = static_cast<SelectionBox*>(item);
                selectedIndexes.emplace_back(selection->getIndex());
            }

            std::sort(selectedIndexes.begin(), selectedIndexes.end());

            presenter->setSelectedTeeth(selectedIndexes);
        });

}

void TeethViewScene::setContextMenu(ContextMenu* contextMenu)
{
    this->contextMenu = contextMenu;
}

void TeethViewScene::setPresenter(StatusPresenter* presenter)
{
    this->presenter = presenter;
}


void TeethViewScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (!contextMenu) return;

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (item != nullptr && item->flags() == item->ItemIsSelectable)
    {
        if (!item->isSelected())
        {
            clearSelection();
            item->setSelected(1);

            auto toothSelected = static_cast<SelectionBox*>(item);

            presenter->setSelectedTeeth(std::vector<int>{toothSelected->getIndex()});
            contextMenu->exec(event->screenPos());
        }
        else
        {
            contextMenu->exec(event->screenPos());
        }
    }

}

void TeethViewScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (item != NULL && !item->isSelected())
    {
        item->setSelected(1);
    }
}

void TeethViewScene::keyPressEvent(QKeyEvent* event)
{
    int lastSelected = 0;
    int firstSelected = 0;
    int selection_length = this->selectedItems().size();
    bool firstSelectedFound = 0;


    for (int i = 0; i < 32; i++)
    {
        if (selectionBox[i]->isSelected())
        {
            if (!firstSelectedFound) { firstSelected = i; firstSelectedFound = 1; }
            lastSelected = i;
        }

    }

    switch (event->key())
    {
    case Qt::Key_Right:
        if (event->modifiers() & Qt::ShiftModifier)
        {
            if (lastSelected < 15) selectionBox[lastSelected + 1]->setSelected(1);
            else if (lastSelected == 15 || firstSelected == 16) selectionBox[lastSelected]->setSelected(1);
            else if (lastSelected > 15 && firstSelected > 15) selectionBox[firstSelected - 1]->setSelected(1);
            else selectionBox[std::max(lastSelected - 1, 0)]->setSelected(1);
            break;
        }
        clearSelection();
        if (lastSelected < 15) selectionBox[lastSelected + 1]->setSelected(1);
        else if (lastSelected == 15 || firstSelected == 16) selectionBox[lastSelected]->setSelected(1);
        else if (lastSelected > 15 && firstSelected > 15) selectionBox[firstSelected - 1]->setSelected(1);
        else selectionBox[std::max(lastSelected - 1, 0)]->setSelected(1);
        break;

    case Qt::Key_Left:
        if (event->modifiers() & Qt::ShiftModifier)
        {
            if (lastSelected < 16) selectionBox[std::max(firstSelected - 1, 0)]->setSelected(1);
            else selectionBox[std::min(lastSelected + 1, 31)]->setSelected(1);
            break;
        }
        clearSelection();
        if (lastSelected < 16) selectionBox[std::max(firstSelected - 1, 0)]->setSelected(1);
        else selectionBox[std::min(lastSelected + 1, 31)]->setSelected(1);
        break;

    case Qt::Key_Down:
        clearSelection();
        if (lastSelected < 16) selectionBox[31 - lastSelected]->setSelected(1);
        else selectionBox[lastSelected]->setSelected(1);
        break;
    case Qt::Key_Up:
        clearSelection();;
        if (lastSelected > 15) selectionBox[31 - lastSelected]->setSelected(1);
        else selectionBox[lastSelected]->setSelected(1);
        break;
    case Qt::Key_Space:
    {
        clearSelection();
        if (selection_length == 0) selectionBox[0]->setSelected(1);
        else if (lastSelected < 31) selectionBox[lastSelected + 1]->setSelected(1);
        else selectionBox[0]->setSelected(1);
        break;
    }

    case Qt::Key_Delete: presenter->changeStatus(StatusAction::removeAll);; break;
    }

      switch (event->nativeVirtualKey()) //shortcut keys for input diagnosis
   {
      case Qt::Key_T :presenter->changeStatus(StatusAction::Temporary); break;
      case Qt::Key_O :presenter->changeStatus(StatusAction::Obturation); break;
      case Qt::Key_C :presenter->changeStatus(StatusAction::Caries); break;
      case Qt::Key_R :presenter->changeStatus(StatusAction::Root); break;
      case Qt::Key_E :presenter->changeStatus(StatusAction::Extraction); break;
      case Qt::Key_S: presenter->changeStatus(StatusAction::Post); break;
      case Qt::Key_P :presenter->changeStatus(StatusAction::Pulpitis); break;
      case Qt::Key_D :presenter->changeStatus(StatusAction::EndoTreatment); break;
      case Qt::Key_F: presenter->changeStatus(StatusAction::Fracture); break;
      case Qt::Key_I :presenter->changeStatus(StatusAction::Implant); break;
      case Qt::Key_L :presenter->changeStatus(StatusAction::Periodontitis); break;
      case Qt::Key_1 :presenter->changeStatus(StatusAction::Mobility1); break;
      case Qt::Key_2 :presenter->changeStatus(StatusAction::Mobility2); break;
      case Qt::Key_3 :presenter->changeStatus(StatusAction::Mobility3); break;
      case Qt::Key_K :presenter->changeStatus(StatusAction::Crown); break;
      case Qt::Key_G :presenter->changeStatus(StatusAction::ApicalLesion); break;
      case Qt::Key_B :presenter->changeStatus(StatusAction::Bridge); break;
      case Qt::Key_A:
          if (event->modifiers() & Qt::ControlModifier)
              for (int i = 0; i < 32; i++) selectionBox[i]->setSelected(1);
          break;
   }
}


void TeethViewScene::display(ToothPaintHint tooth)
{

    toothGraphic[tooth.idx]->setToothGraphic
    (
        toothPainter.paintTooth(tooth)
    );
}

void TeethViewScene::display(const BridgesPaintHint& bridges)
{
    auto[bridgeU, bridgeL] = bridgePainter.getBridgePair(bridges);

    upperBridge->setTexture(bridgeU);
    lowerBridge->setTexture(bridgeL);

}

void TeethViewScene::setSelectedTeeth(const std::vector<int> &selectedTeeth)
{
    clearSelection();
    //read access violation
    for (int i : selectedTeeth){
        selectionBox[i]->setSelected(1);
    }
}

void TeethViewScene::setProcedures(std::vector<RowData> procedures)
{
    for (auto& t : toothGraphic)
    {
        t->setProcedure(false);
    }

    for (auto& p : procedures)
    {
        if (p.toothIdx != -1)
            toothGraphic[p.toothIdx]->setProcedure(true);
        
    }
}

TeethViewScene::~TeethViewScene()
{
    this->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (ListInstance vector throws exception)
}
