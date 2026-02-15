#include "TeethViewScene.h"

#include <algorithm>
#include <vector>

#include <QApplication>
#include <QGuiApplication>
#include <QtGlobal>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QGraphicsView>

#include "Model/Dental/Dental.h"
#include "Presenter/ListPresenter.h"
#include "View/Graphics/ToothPainter.h"
#include "View/SubWidgets/ContextMenu.h"
#include "View/Graphics/SimpleToothItem.h"
#include "View/Theme.h"

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
            selectableItems[i] = selectionBox[i];
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

    //INIT simple tooth
    {
        posY = 90;
        posX = 12;

        for (int i = 0; i < 32; i++)
        {
            if (i == 16)
                posY += simpleTooth[15]->boundingRect().height();

            simpleTooth[i] = new SimpleToothItem(i);
            simpleTooth[i]->setZValue(3);
            simpleTooth[i]->setPos(posX, posY);
            addItem(simpleTooth[i]);

            const qreal stepX = simpleTooth[i]->boundingRect().width();

            if (i < 15)
                posX += stepX;
            else if (i > 15 )
                posX -= stepX;
        }
    }
    
    title = new QGraphicsTextItem(QStringLiteral("Дентален статус"));
    QFont f = title->font();
    f.setBold(true);
    f.setPointSize(18);
#ifdef Q_OS_OSX
    f.setPointSize(24);
#endif
    title->setFont(f);
    title->setDefaultTextColor(Theme::fontRed);

    const qreal titleY = 16;
    const qreal titleX = 255;

    title->setPos(titleX, titleY);
    addItem(title);
    
    initStatusLegend();

    connect(this, &QGraphicsScene::selectionChanged, [=, this]
    {
        std::vector<int> selectedIndexes;
        selectedIndexes.reserve(32);

        for (QGraphicsItem* item : selectedItems())
        {
            auto selection = static_cast<SelectionBox*>(item);
            selectedIndexes.emplace_back(selection->getIndex());
        }

        std::sort(selectedIndexes.begin(), selectedIndexes.end());

        if(presenter){
            presenter->setSelectedTeeth(selectedIndexes);
        }

        if(contextMenu){
            contextMenu->setSelection(selectedIndexes.size() == 1);
        }
            
    });

    selectionBox[0]->setNeighbours(nullptr, selectionBox[1]);

    for(int i = 1; i < 15; i++){
        selectionBox[i]->setNeighbours(selectionBox[i-1], selectionBox[i+1]);
    }

    selectionBox[15]->setNeighbours(selectionBox[14], nullptr);

    selectionBox[16]->setNeighbours(selectionBox[17], nullptr);

    for(int i = 17; i < 31; i++){
        selectionBox[i]->setNeighbours(selectionBox[i+1], selectionBox[i-1]);
    }

    selectionBox[31]->setNeighbours(nullptr, selectionBox[30]);

    setSimpleView(s_default_simple_view);
    drawFocused(false);
}

void TeethViewScene::setContextMenu(ContextMenu* contextMenu)
{
    this->contextMenu = contextMenu;
}

void TeethViewScene::setPresenter(ListPresenter* presenter)
{
    this->presenter = presenter;
}

std::vector<int> TeethViewScene::getSelectedTeethIdx()
{
    std::vector<int> result;

    result.reserve(32);

    for(auto& item : items()){
        if(!item->isSelected()) continue;
        result.push_back(static_cast<SelectionBox*>(item)->getIndex());
    }

    return result;
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
    if (views().isEmpty() || views().at(0)->dragMode() != QGraphicsView::RubberBandDrag) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::RightButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

        if (item && item->isSelected()) {
            event->accept();
            return;
        }

        if (item && !item->isSelected()) {
            clearSelection();
            item->setSelected(true);
            event->accept();
            return;
        }

        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    const bool ctrl = (event->modifiers() & Qt::ControlModifier);

    if (ctrl) {
        if (QGraphicsItem* item = itemAt(event->scenePos(), QTransform())) {
            item->setSelected(!item->isSelected());
        }
        event->ignore(); // allow rubberband on drag
        return;
    }

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item != nullptr && !item->isSelected()) {
        item->setSelected(true);
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

		emit toothDoubleClicked(t->getIndex());
    }
}

void TeethViewScene::initStatusLegend()
{
    using Item = std::pair<QString, QString>;

    std::vector<Item> items;
    items.reserve(Dental::status_symbols.size());
    for (const auto& kv : Dental::status_symbols)
        items.emplace_back(QString::fromStdString(kv.first),
            QString::fromStdString(kv.second));

    int cols = 3;
    int rows = int((items.size() + cols - 1) / cols);

    int keyMax = 0;
    for (const auto& it : items)
        keyMax = std::max(keyMax, int(it.first.size()));

    constexpr int kCol0Count = 10;
    constexpr int kCol1Count = 10;
    constexpr int kCol2Count = 7;

    QString colText[3];

    int start = 0;
    int colCounts[3] = { 10, 10, 7 };

    for (int c = 0; c < 3; ++c)
    {
        const int count = colCounts[c];

        QStringList lines;
        lines.reserve(count);

        int end = std::min(start + count, int(items.size()));
        for (int i = start; i < end; ++i)
        {
            QString k = items[i].first;
            if (k.size() < keyMax)
                k += QString(keyMax - k.size(), QLatin1Char(' '));

            lines << ("<b>" + k + "</b> - " + items[i].second + "<br>");
        }

        colText[c] = lines.join(QLatin1Char('\n'));
        start += count;
    }

    qreal gridX = 15.0;
    qreal gridY = 15.0;
    qreal cellW = 43.0;
    qreal cellH = 70.0;
    qreal gapY = 100.0;

    qreal gridW = 16.0 * cellW;
    qreal gridH = cellH + gapY + cellH;

    qreal legendY = gridY + gridH + 25.0;
    qreal colW = gridW / 3.0;

    for (int c = 0; c < 3; ++c)
    {
        if (!legend[c])
        {
            legend[c] = new QGraphicsTextItem();
            legend[c]->setZValue(60);

            QFont f = legend[c]->font();
            f.setPointSize(9);
#ifdef Q_OS_OSX
            f.setPointSize(13);
#endif
            legend[c]->setFont(f);

            legend[c]->setDefaultTextColor(Theme::fontTurquoise);

            addItem(legend[c]);
        }

        legend[c]->setHtml(colText[c]);
        legend[c]->setTextWidth(colW - 10.0);
        legend[c]->setPos(gridX + c * colW, legendY);
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
        {1066, Qt::Key_Y},
        {1053, Qt::Key_N},
        {1047, Qt::Key_Z},
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
    qDebug() << event;
    int lastSelected = 0;
    int firstSelected = 0;
    int selection_length = this->selectedItems().size();
    bool firstSelectedFound = 0;


    for (int i = 0; i < 32; i++)
    {
        if (selectableItems[i]->isSelected())
        {
            if (!firstSelectedFound) { firstSelected = i; firstSelectedFound = 1; }
            lastSelected = i;
        }
    }

    switch (event->key())
    {
        case Qt::Key_Return: emit returnPressed(); if(presenter) presenter->addProcedure(); break;
        case Qt::Key_Right:
            if (event->modifiers() & Qt::ShiftModifier)
            {
                if (lastSelected < 15) selectableItems[lastSelected + 1]->setSelected(1);
                else if (lastSelected == 15 || firstSelected == 16) selectableItems[lastSelected]->setSelected(1);
                else if (lastSelected > 15 && firstSelected > 15) selectableItems[firstSelected - 1]->setSelected(1);
                else selectableItems[std::max(lastSelected - 1, 0)]->setSelected(1);
                break;
            }
            clearSelection();
            if (lastSelected < 15) selectableItems[lastSelected + 1]->setSelected(1);
            else if (lastSelected == 15 || firstSelected == 16) selectableItems[lastSelected]->setSelected(1);
            else if (lastSelected > 15 && firstSelected > 15) selectableItems[firstSelected - 1]->setSelected(1);
            else selectableItems[std::max(lastSelected - 1, 0)]->setSelected(1);
            break;

        case Qt::Key_Left:
            if (event->modifiers() & Qt::ShiftModifier)
            {
                if (lastSelected < 16) selectableItems[std::max(firstSelected - 1, 0)]->setSelected(1);
                else selectableItems[std::min(lastSelected + 1, 31)]->setSelected(1);
                break;
            }
            clearSelection();
            if (lastSelected < 16) selectableItems[std::max(firstSelected - 1, 0)]->setSelected(1);
            else selectableItems[std::min(lastSelected + 1, 31)]->setSelected(1);
            break;

        case Qt::Key_Down:
            clearSelection();
            if (lastSelected < 16) selectableItems[31 - lastSelected]->setSelected(1);
            else selectableItems[lastSelected]->setSelected(1);
            break;
        case Qt::Key_Up:
            clearSelection();;
            if (lastSelected > 15) selectableItems[31 - lastSelected]->setSelected(1);
            else selectableItems[lastSelected]->setSelected(1);
            break;
        case Qt::Key_Space:
        {
            setSimpleView(!m_simple_view);
            break;
        }

        case Qt::Key_Delete: if(presenter) presenter->setOther(OtherInputs::removeAll);; break;
    }

    if (!presenter) return;

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
      case Qt::Key_Y:presenter->setToothStatus(StatusType::General, DefectiveRestoration); break;
      case Qt::Key_U:presenter->setToothStatus(StatusType::General, NonCariesLesion); break;
      case Qt::Key_N:presenter->setToothStatus(StatusType::General, Necrosis); break;
      case Qt::Key_Z: presenter->setToothStatus(StatusType::General, Resorption); break;
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
              for (int i = 0; i < 32; i++) selectableItems[i]->setSelected(1);
          break;
      default: return;
   }
}

void TeethViewScene::display(const ToothPaintHint& tooth)
{
    toothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalOcclusal(tooth));
    simpleTooth[tooth.idx]->setData(tooth.num, tooth.statuses);

    if (tooth.dsn)
    {
        dsnToothGraphic[tooth.idx]->setToothPixmap(ToothPainter::getBuccalOcclusal(*tooth.dsn));
    }
    else
    {
        dsnToothGraphic[tooth.idx]->drawEmpty();
    }

    selectableItems[tooth.idx]->setToolTip(tooth.toolTip.c_str());
}


void TeethViewScene::setSelectedTeeth(const std::vector<int> &selectedTeeth)
{
    clearSelection();

    for (int i : selectedTeeth){ //read access violation ???
        selectableItems[i]->setSelected(1);
    }
}

void TeethViewScene::setProcedures(std::vector<int> teeth_procedures)
{
    for (auto& t : toothGraphic) t->setProcedure(false);

    for (auto& t : simpleTooth) t->setProcedure(false);

    for (auto& p : teeth_procedures) {
        if (p == -1) continue;
        simpleTooth[p]->setProcedure(true);
        toothGraphic[p]->setProcedure(true);
    }
       
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
    for(auto selBox : selectableItems){
        selBox->drawFocused(focused);
    }

    for (auto l : legend) {
        l->setDefaultTextColor(focused ? Theme::fontTurquoise : Theme::fontTurquoiseClicked);
    }
}

void TeethViewScene::setSimpleView(bool simple)
{
    s_default_simple_view = simple;

    std::vector<int> selected;
    selected.reserve(32);

    for (int i = 0; i < 32; ++i)
    {
        if (selectableItems[i]->isSelected())
            selected.push_back(i);
    }

    for (int i = 0; i < 32; ++i)
    {
        toothGraphic[i]->setVisible(!simple);
        dsnToothGraphic[i]->setVisible(!simple);
        selectionBox[i]->setVisible(!simple);
        selectionBox[i]->resetHoverState();
        simpleTooth[i]->setVisible(simple);
        simpleTooth[i]->resetHoverState();
        title->setVisible(simple);
        
        for (auto l : legend) {
            l->setVisible(simple);
        }

        selectableItems[i] = simple
            ? static_cast<SelectableGraphicsItem*>(simpleTooth[i])
            : static_cast<SelectableGraphicsItem*>(selectionBox[i]);
    }

    clearSelection();

    for (int idx : selected)
        if (selectableItems[idx])
            selectableItems[idx]->setSelected(true);

    m_simple_view = simple;

    drawFocused(true);
}

TeethViewScene::~TeethViewScene()
{
    this->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (ListInstance vector throws exception)
}
