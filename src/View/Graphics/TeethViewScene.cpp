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
        if (!presenter) { return; }

        std::vector<int> selectedIndexes;
        selectedIndexes.reserve(32);

        for (QGraphicsItem* item : selectedItems())
        {
            auto selection = static_cast<SelectionBox*>(item);
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
    if (!presenter) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

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

        if (presenter) {
            presenter->addProcedure();
        }

		emit tooethDoubleClicked(t->getIndex());
    }
}


int TeethViewScene::keyCodeMapper(QKeyEvent *e)
{
    #if defined(Q_OS_WIN)
        return e->nativeVirtualKey();
    #elif defined (Q_OS_MAC)

    const static std::map<int, int> cyrillic_map{
        {1058, Qt::Key_T},
        {1054, Qt::Key_O},
        {1062, Qt::Key_C},
        {1056, Qt::Key_R},
        {1045, Qt::Key_E},
        {1071, Qt::Key_Q},
        {1055, Qt::Key_P},
        {1044, Qt::Key_D},
        {1060, Qt::Key_F},
        {1048, Qt::Key_I},
        {1051, Qt::Key_L},
        {1050, Qt::Key_K},
        {1043, Qt::Key_G},
        {1041, Qt::Key_B},
        {1052, Qt::Key_M},
        {1057, Qt::Key_S},
        {1059, Qt::Key_U},
        {1068, Qt::Key_X},
        {1061, Qt::Key_H},
        {1046, Qt::Key_V},
        {1070, Qt::Key_Y},
        {1085, Qt::Key_N},
        {1079, Qt::Key_Z},
        {1081, Qt::Key_J},
        {1040, Qt::Key_A}
    };

    auto result = e->key();

    if(result >1000 && cyrillic_map.count(result)){
        result = cyrillic_map.at(result);
    }

    return result;

    #else

        auto result = e->key();

        if(result >1000) result -= 975;

        return static_cast<Qt::Key>(result);

    #endif
}


void TeethViewScene::keyPressEvent(QKeyEvent* event)
{
    if (!presenter) return;

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
        case Qt::Key_Return: presenter->addProcedure(); break;
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
      case Qt::Key_Q :presenter->setToothStatus(StatusType::General, Post); break;
      case Qt::Key_P :presenter->setToothStatus(StatusType::General, Pulpitis); break;
      case Qt::Key_D :presenter->setToothStatus(StatusType::General, RootCanal); break;
      case Qt::Key_F :presenter->setToothStatus(StatusType::General, Fracture); break;
      case Qt::Key_I :presenter->setToothStatus(StatusType::General, Implant); break;
      case Qt::Key_L :presenter->setToothStatus(StatusType::General, Periodontitis); break;
#ifndef DISABLE_NEW_DENTAL_STATUSES
      case Qt::Key_Y:presenter->setToothStatus(StatusType::General, DefectiveRestoration); break;
      case Qt::Key_U:presenter->setToothStatus(StatusType::General, NonCariesLesion); break;
      case Qt::Key_N:presenter->setToothStatus(StatusType::General, Necrosis); break;
      case Qt::Key_Z: presenter->setToothStatus(StatusType::General, Resorption); break;
#endif
      case Qt::Key_0 :presenter->setToothStatus(StatusType::General, Mobility); break;
      case Qt::Key_1 :presenter->setToothStatus(StatusType::Mobility, 0); break;
      case Qt::Key_2 :presenter->setToothStatus(StatusType::Mobility, 1); break;
      case Qt::Key_3 :presenter->setToothStatus(StatusType::Mobility, 2); break;
      case Qt::Key_K :presenter->setToothStatus(StatusType::General, Crown); break;
      case Qt::Key_G :presenter->setToothStatus(StatusType::General, ApicalLesion); break;
      case Qt::Key_B :presenter->setToothStatus(StatusType::General, Bridge); break;
      case Qt::Key_M: presenter->setToothStatus(StatusType::General, Impacted); break;
      case Qt::Key_S: presenter->setToothStatus(StatusType::General, Splint); break;
      case Qt::Key_J: presenter->setToothStatus(StatusType::General, HasSupernumeral); break;
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

void TeethViewScene::drawFocused(bool focused)
{
    for(auto selBox : selectionBox){
        selBox->drawFocused(focused);
    }

}

TeethViewScene::~TeethViewScene()
{
    this->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (ListInstance vector throws exception)
}
