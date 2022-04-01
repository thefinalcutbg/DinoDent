#include "ProcedureTable.h"
#include "View/Theme.h"
#include <QHeaderView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPainterPath>
#include <QPainter>
#include <QStyledItemDelegate>

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

ProcedureTable::ProcedureTable(QWidget* parent)
    : QTableView(parent), header(Qt::Orientation::Horizontal)
{
    
    setHorizontalHeader(&header);
    installEventFilter(this);
    setItemDelegate(new NoFocusDelegate);
    horizontalHeader()->setFixedHeight(50);

    setFrameShape(QFrame::Shape::NoFrame);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    horizontalHeader()->setHighlightSections(false);
    
    verticalHeader()->setVisible(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    viewport()->setFocusPolicy(Qt::StrongFocus);
    setShowGrid(true);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(50);
    //setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWordWrap(true);



    
}

void ProcedureTable::setAmbListLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel(model()->rowCount());});
    hideColumn(0);
    setColumnWidth(1, 69);
    setColumnWidth(2, 200);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 49);
    setColumnWidth(6, 69);
    hideColumn(7);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    //setShowGrid(false);
}

void ProcedureTable::setProcedureHistoryLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel(model()->rowCount());});

    hideColumn(0);
    setColumnWidth(1, 69);
    setColumnWidth(2, 200);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 49);
    setColumnWidth(6, 69);
    setColumnWidth(7, 140);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
}

void ProcedureTable::setProcedurePrintSelectLayout()
{
    setFocusPolicy(Qt::NoFocus);
    setColumnWidth(0, 110);
    setColumnWidth(1, 200);
    setColumnWidth(2, 25);
    setColumnWidth(3, 300);
    setColumnWidth(4, 49);
    setColumnWidth(5, 80);
    verticalHeader()->setDefaultSectionSize(20);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  //  horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  //  horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
  //  horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
}

void ProcedureTable::setBusinessOperationLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel(model()->rowCount());});
    setColumnWidth(0, 50);
    setColumnWidth(1, 50);
    setColumnWidth(2, 400);
    setColumnWidth(3, 50);
    setColumnWidth(4, 75);
    setColumnWidth(5, 75);
}

void ProcedureTable::fitToModel(int rows) //not working correctly yet
{
    int tableHeight = rows*50 + horizontalHeader()->height() + 10;

    if (!rows) {
        tableHeight += 50;
    }
    
    setFixedHeight(tableHeight);

}

void ProcedureTable::keyPressEvent(QKeyEvent* event)
{

    switch (event->key())
    {
    case Qt::Key_Delete:
        emit deletePressed();
        break;
    default:
        QTableView::keyPressEvent(event);
    }
}

void ProcedureTable::paintEvent(QPaintEvent* e)
{
    QTableView::paintEvent(e);

    constexpr int footerHeight = 10;

    int h = viewport()->height();
    int w = viewport()->width();

    QPainter painter(viewport());

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    painter.fillRect(QRect(0, h - footerHeight +1, w, footerHeight), Theme::background);

    QPainterPath path = Theme::getHalfCurvedPath(footerHeight, w);

    painter.translate(0, h-1);
    painter.rotate(-90);

    QColor background(Theme::sectionBackground);
/*
    //buggy:
    if (selectionModel()->currentIndex().row() == model()->rowCount() - 1) {
        background = Theme::background;
    }
*/
    painter.fillPath(path, background);

    painter.setPen(QPen(Theme::border));
    painter.drawPath(path);

    painter.resetTransform();

    painter.drawLine(0, 0, 0, h - 8);
    painter.drawLine(w - 1, 0, w - 1, h - 8);
   
    QPen eraser(background);
    eraser.setWidth(3);
    painter.setPen(eraser);
    painter.drawLine(2, h- footerHeight-1, w - 3, h - footerHeight-1);

    QPen border(Theme::border);
    border.setWidthF(1*devicePixelRatioFScale());
    painter.setPen(border);

    

    if (model() != nullptr && model()->rowCount()) {

        double xPos = 0;
        bool skipFirst = true;

        for (int i = 1; i < model()->columnCount(); i++)
        {
            
            if (horizontalHeader()->isSectionHidden(i)) {
                continue;
            }

            xPos += horizontalHeader()->sectionSize(i - 1);

            if (skipFirst) { 
                skipFirst = false; 
                continue;
            }

            painter.drawLine(QPointF(xPos-1, 0), QPointF(xPos, h + footerHeight));
            
        }

    }


    painter.end();

}

int ProcedureTable::selectedRow()
{
    if (selectionModel() == nullptr) return -1;

    return selectionModel()->currentIndex().row();


}

ProcedureTable::~ProcedureTable()
{
}

void ProcedureHeader::paintEvent(QPaintEvent* e)
{
    QPainter painter(this->viewport());

    int radius = 6;

    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Theme::background);

    QPainterPath path = Theme::getHalfCurvedPath(height(), width()-1);;

    painter.translate(width()-1, 1);
    painter.rotate(90);
    
    painter.fillPath(path, Theme::sectionBackground);

    painter.setPen(Theme::border);
    painter.drawPath(path);

    painter.resetTransform();

    painter.setPen(Theme::fontTurquoiseClicked);

    painter.drawLine(0, height(), width() - 1, height());

    QFont font("Segoe UI");
    font.setBold(true);

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
