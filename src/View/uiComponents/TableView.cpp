#include "TableView.h"
#include "View/Theme.h"
#include <QHeaderView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPainterPath>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QMenu>


class NoFocusDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem itemOption(option);
        if (itemOption.state & QStyle::State_HasFocus)
            itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
        QStyledItemDelegate::paint(painter, itemOption, index);
    }

};

TableView::TableView(QWidget* parent)
    : QTableView(parent), header(Qt::Orientation::Horizontal)
{
    setHorizontalHeader(&header);
    installEventFilter(this);
    setItemDelegate(new NoFocusDelegate);
    horizontalHeader()->setFixedHeight(30);

    setFrameShape(QFrame::Shape::NoFrame);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    horizontalHeader()->setHighlightSections(false);
    
    verticalHeader()->setVisible(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   // viewport()->setFocusPolicy(Qt::StrongFocus);
    setShowGrid(true);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(50);
    //setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropOverwriteMode(false);
    setDropIndicatorShown(true);

    setWordWrap(true);



    menu = new QMenu(this);

    menu->setStyleSheet(Theme::getPopupMenuStylesheet());

    QAction* action;

    action = (new QAction("Редактирай", menu));
    connect(action, &QAction::triggered, this, [&] { emit editPressed(selectedRow()); });
    action->setIcon(QIcon(":/icons/icon_edit.png"));
    menu->addAction(action);
    action = (new QAction("Изтрий", menu));
    connect(action, &QAction::triggered, this, [&] { emit deletePressed(selectedRow()); });
    action->setIcon(QIcon(":/icons/icon_remove.png"));
    menu->addAction(action);

    connect(this, &QTableView::doubleClicked, this, [&] { emit editPressed(selectedRow()); });

    connect(this, &QWidget::customContextMenuRequested, this, [&](const QPoint& p){
        if (!isEnabled()) return;
        if (this->selectedRow() == -1) return;
        menu->popup(viewport()->mapToGlobal(p));

    });
    
}

void TableView::setAmbListLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, this, [&] { fitToModel();});
    hideColumn(0);
    setColumnWidth(1, 105);
    setColumnWidth(2, 250);
    setColumnWidth(3, 65);
    setColumnWidth(4, 250);
    setColumnWidth(5, 70);
    setColumnWidth(6, 100);
    hideColumn(7);

    horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

    setShowGrid(false);
}

void TableView::setProcedureHistoryLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, this, [&] { fitToModel();});

    hideColumn(0);
    setColumnWidth(1, 100);
    setColumnWidth(2, 200);
    setColumnWidth(3, 65);
    setColumnWidth(4, 300);
    setColumnWidth(5, 100);
    setColumnWidth(6, 100);
    setColumnWidth(7, 100);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    setShowGrid(false);
}

void TableView::setProcedurePrintSelectLayout()
{
    setFocusPolicy(Qt::NoFocus);
    setColumnWidth(0, 110);
    setColumnWidth(1, 200);
    setColumnWidth(2, 65);
    setColumnWidth(3, 300);
    setColumnWidth(4, 100);
    setColumnWidth(5, 100);
    verticalHeader()->setDefaultSectionSize(20);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    setShowGrid(false);
}

void TableView::setBusinessOperationLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, this, [&] { fitToModel();});
    setColumnWidth(0, 50);
    setColumnWidth(1, 100);
    setColumnWidth(2, 350);
    setColumnWidth(3, 65);
    setColumnWidth(4, 100);
    setColumnWidth(5, 100);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    setShowGrid(false);
}

void TableView::setMedicationLayot()
{
    connect(model(), &QAbstractTableModel::dataChanged,  this, [&] { fitToModel();});

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    setShowGrid(false);
    setWordWrap(true);
}

void TableView::setStatisticLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, this, [&] { fitToModel();});

    setColumnWidth(1, 70);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    setShowGrid(false);
}

void TableView::setPISActivitiesLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, this, [&] { fitToModel();});

    hideColumn(0);
    setColumnWidth(1, 69);
    setColumnWidth(2, 200);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 100);
    hideColumn(6);
   // hideColumn(7);
    hideColumn(7);
    verticalHeader()->setDefaultSectionSize(20);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
 //   horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    setShowGrid(false);
}

void TableView::fitToModel() //not working correctly yet
{
    auto rows = model()->rowCount();

    int tableHeight = rows*50 + horizontalHeader()->height() + 10;

    if (!rows) {
        tableHeight += 50;
    }
    
    setFixedHeight(tableHeight);

}

void TableView::enableContextMenu(bool enabled)
{
    setContextMenuPolicy(enabled ? Qt::CustomContextMenu : Qt::NoContextMenu);
}

void TableView::keyPressEvent(QKeyEvent* event)
{

    switch (event->key())
    {
    case Qt::Key_Delete:
        emit deletePressed(selectedRow());
        break;
    default:
        QTableView::keyPressEvent(event);
    }
}

void TableView::paintEvent(QPaintEvent* e)
{
    // Let Qt paint items/selection/etc first
    QTableView::paintEvent(e);

    auto* m = model();
    if (!m) return;

    // During model resets/layout, header counts can lag model counts
    const int colCount = qMin(m->columnCount(), horizontalHeader()->count());
    const int rowCount = qMin(m->rowCount(),    verticalHeader()->count());
    if (colCount <= 0 || rowCount <= 0) return;

    const int h = viewport()->height() - 10;
    const int w = viewport()->width();

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(hasFocus() ? Theme::mainBackgroundColor : Theme::border);
    pen.setCosmetic(true);
    pen.setWidth(1);
    painter.setPen(pen);

    // columns
    double xPos = 0;
    bool firstVisible = true;
    for (int i = 0; i < colCount; ++i) {
        if (horizontalHeader()->isSectionHidden(i)) continue;

        const int sz = horizontalHeader()->sectionSize(i);
        if (firstVisible) {
            firstVisible = false;
            xPos += sz;
            continue;
        }
        painter.drawLine(QPointF(xPos - 1, 0), QPointF(xPos - 1, h));
        xPos += sz;
    }

    // rows
    double yPos = 0;
    for (int i = 1; i < rowCount; ++i) {
        if (verticalHeader()->isSectionHidden(i)) continue;

        const int prevSz = verticalHeader()->sectionSize(i - 1);
        yPos += prevSz;
        painter.drawLine(QPointF(1, yPos), QPointF(w - 1, yPos));
    }

    // last line after last visible row (optional)
    double yEnd = 0;
    for (int r = 0; r < rowCount; ++r) {
        if (verticalHeader()->isSectionHidden(r)) continue;
        yEnd += verticalHeader()->sectionSize(r);
    }
    // if you enable it:
     if (yEnd >= 0 && yEnd <= h) painter.drawLine(QPointF(1, yEnd), QPointF(w - 1, yEnd));
}


Q_INVOKABLE int TableView::selectedRow() const
{
    QItemSelectionModel* selection = selectionModel();
    return selection->hasSelection() ? selection->selectedRows().front().row() : -1;
}

void TableView::dropEvent(QDropEvent* e)
{
    if (e->source() != this || e->dropAction() != Qt::MoveAction)
        return;

   // int dragRow = selectedRow();

    QTableView::dropEvent(e);  // m_dropRow is set by inserted row

    emit rowDragged();
}

int TableView::selectedRow()
{
    if (selectionModel() == nullptr) return -1;

    return selectionModel()->currentIndex().row();
}

TableView::~TableView()
{
}

void TableViewHeader::paintEvent(QPaintEvent*)
{
    QPainter painter(this->viewport());

    painter.setRenderHint(QPainter::Antialiasing);

    auto focused = static_cast<QWidget*>(parent())->hasFocus();

    QPen borderPen(focused ? Theme::mainBackgroundColor : Theme::border);
    borderPen.setWidth(2);
    borderPen.setCosmetic(true);

    painter.setPen(borderPen);

    painter.drawLine(0, height(), width() - 1, height());

    QFont font;
    font.setBold(true);

    borderPen.setColor(focused ? Theme::fontTurquoise : Theme::fontTurquoiseClicked);
    painter.setPen(borderPen);

    painter.setFont(font);

    int xPos{ 0 };

    if (model() != nullptr)
    {
        for (int i = 0; i < model()->columnCount(); i++) {

            if (isSectionHidden(i)) {
                continue;
            }

            QRect rect{
                xPos,
                0,
                sectionSize(i),
                height()
            };

            painter.drawText(rect, Qt::AlignCenter, model()->headerData(i, Qt::Orientation::Horizontal, Qt::DisplayRole).toString());

            xPos += sectionSize(i);

        }

    }

    painter.end();

    //QHeaderView::paintEvent(e);
}
