#include "TeethViewScene.h"

#include <QApplication>
#include <QGuiApplication>
#include <QtGlobal>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "Presenter/ListPresenter.h"
#include "View/Graphics/ToothPainter.h"
#include "View/SubWidgets/ContextMenu.h"
#include "ToothGraphicsItem.h"
#include "SelectionBox.h"


using namespace Dental;

TeethViewScene::TeethViewScene(QObject *parent)
    : QGraphicsScene(parent), presenter(nullptr), contextMenu(nullptr)
{
    int posY = 15;
    int posX = 15;

    int selectionBox_posY = posY;

    for(int i = 0; i<32; i++)
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

        {
            dsnToothGraphic[i] = new ToothGraphicsItem(i);
            dsnToothGraphic[i]->setZValue(0);

        
            int dnsPos = (i < 8 || i > 23) ?
                posX - (dsnToothGraphic[i]->boundingRect().width() / 2)
                :
                posX + (dsnToothGraphic[i]->boundingRect().width() / 2);

            dsnToothGraphic[i]->setPos(dnsPos, posY);
            addItem(dsnToothGraphic[i]);
        }

        {

            selectionBox[i] = new SelectionBox(i);
            selectionBox[i]->setZValue(2);
            selectionBox[i]->setPos(posX, selectionBox_posY);
            addItem(selectionBox[i]);

        }

        if (i < 15)
            posX += toothGraphic[i]->boundingRect().width();
        else if(i > 15)
            posX -= toothGraphic[i]->boundingRect().width();

        if (i == 18) posX += 18;
        else if (i == 28) posX -= 18;
    }

    connect(this, &QGraphicsScene::selectionChanged, [=, this]
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
            contextMenu->setSelection(selectedIndexes.size() == 1);
            
        });

}

void TeethViewScene::setContextMenu(ContextMenu* contextMenu)
{
    this->contextMenu = contextMenu;
}

void TeethViewScene::setPresenter(ListPresenter* presenter)
{
    this->presenter = presenter;

    /* //improves ram usage, but slows down the whole program
    if(!presenter)
        for (auto tooth : toothGraphic)
        {
            tooth->setToothPixmap(QPixmap());
        }
    */
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

    //ctr bug:
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }

    //menu bug:
    if (event->button() != Qt::LeftButton || 
        QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (item != NULL && !item->isSelected())
    {
        item->setSelected(1);
    }
}



void TeethViewScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* Event)
{
    QGraphicsItem* item = itemAt(Event->scenePos(), QTransform());

    if (item != NULL && item->flags().testFlag(QGraphicsItem::ItemIsSelectable))
    {
        auto* t = static_cast<SelectionBox*>(item);
        presenter->openDetails(t->getIndex());
    }
}


int TeethViewScene::keyCodeMapper(QKeyEvent *e)
{
    #ifdef Q_OS_WIN
        return e->nativeVirtualKey();
    #else

        auto result = e->key();

        if(result >1000) result -= 975;

        return static_cast<Qt::Key>(result);

    #endif
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

        case Qt::Key_Delete: presenter->setOther(OtherInputs::removeAll);; break;
    }

    switch (keyCodeMapper(event)) //shortcut keys for input diagnosis
    {
      case Qt::Key_T :presenter->setToothStatus(StatusType::General, Temporary); break;
      case Qt::Key_O :presenter->setToothStatus(StatusType::General, Restoration); break;
      case Qt::Key_C :presenter->setToothStatus(StatusType::General, Caries); break;
      case Qt::Key_R :presenter->setToothStatus(StatusType::General, Root); break;
      case Qt::Key_E :presenter->setToothStatus(StatusType::General, Missing); break;
      case Qt::Key_Q: presenter->setToothStatus(StatusType::General, Post); break;
      case Qt::Key_P :presenter->setToothStatus(StatusType::General, Pulpitis); break;
      case Qt::Key_D :presenter->setToothStatus(StatusType::General, RootCanal); break;
      case Qt::Key_F: presenter->setToothStatus(StatusType::General, Fracture); break;
      case Qt::Key_I :presenter->setToothStatus(StatusType::General, Implant); break;
      case Qt::Key_L :presenter->setToothStatus(StatusType::General, Periodontitis); break;
      case Qt::Key_0: presenter->setToothStatus(StatusType::General, Mobility); break;
      case Qt::Key_1 :presenter->setToothStatus(StatusType::Mobility, 0); break;
      case Qt::Key_2:presenter->setToothStatus(StatusType::Mobility, 1); break;
      case Qt::Key_3 :presenter->setToothStatus(StatusType::Mobility, 2); break;
      case Qt::Key_K :presenter->setToothStatus(StatusType::General, Crown); break;
      case Qt::Key_G :presenter->setToothStatus(StatusType::General, ApicalLesion); break;
      case Qt::Key_B :presenter->setToothStatus(StatusType::General, Bridge); break;
      case Qt::Key_M: presenter->setToothStatus(StatusType::General, Impacted); break;
      case Qt::Key_S: presenter->setToothStatus(StatusType::General, Splint); break;
      case Qt::Key_U: presenter->setToothStatus(StatusType::General, HasSupernumeral); break;
      case Qt::Key_X: presenter->setToothStatus(StatusType::General, Denture); break;
      case Qt::Key_H: presenter->setToothStatus(StatusType::General, Healthy); break;
      case Qt::Key_V: presenter->setToothStatus(StatusType::General, Calculus); break;
      case Qt::Key_A:
          if (event->modifiers() & Qt::ControlModifier)
              for (int i = 0; i < 32; i++) selectionBox[i]->setSelected(1);
          break;
      default: return;
   }
}

void TeethViewScene::display(const ToothPaintHint& tooth)
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

    selectionBox[tooth.idx]->setToolTip(tooth.toolTip.c_str());
}


void TeethViewScene::setSelectedTeeth(const std::vector<int> &selectedTeeth)
{
    clearSelection();

    for (int i : selectedTeeth){ //read access violation ???
        selectionBox[i]->setSelected(1);
    }
}

void TeethViewScene::setProcedures(std::vector<int> teeth_procedures)
{
    for (auto& t : toothGraphic)
        t->setProcedure(false);

    for (auto& p : teeth_procedures)
        if (p != -1) toothGraphic[p]->setProcedure(true);
       
}

void TeethViewScene::setNotes(const std::array<std::string, 32>& notes)
{
    for (int i = 0; i < notes.size(); i++)
    {
        toothGraphic[i]->setNote(!notes[i].empty());
    }
    
}

TeethViewScene::~TeethViewScene()
{
    this->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (ListInstance vector throws exception)
}
