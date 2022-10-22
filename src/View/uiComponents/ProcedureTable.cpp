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
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel();});
    hideColumn(0);
    setColumnWidth(1, 69);
    setColumnWidth(2, 280);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 49);
    setColumnWidth(6, 70);
   // setColumnWidth(7, 69);
    hideColumn(7);

    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    setShowGrid(false);
}

void ProcedureTable::setProcedureHistoryLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel();});

    hideColumn(0);
   
    setColumnWidth(1, 69);
    setColumnWidth(2, 200);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 49);
    setColumnWidth(6, 70);
    //setColumnWidth(7, 69);
    setColumnWidth(7, 140);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    setShowGrid(false);
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
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    setShowGrid(false);
}

void ProcedureTable::setBusinessOperationLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel();});
    setColumnWidth(0, 50);
    setColumnWidth(1, 50);
    setColumnWidth(2, 400);
    setColumnWidth(3, 50);
    setColumnWidth(4, 75);
    setColumnWidth(5, 75);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    setShowGrid(false);
}

void ProcedureTable::setMedicationLayot()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel();});

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    setShowGrid(false);
    setWordWrap(true);
}

void ProcedureTable::setPISActivitiesLayout()
{
    connect(model(), &QAbstractTableModel::dataChanged, [=] { fitToModel();});

    hideColumn(0);
    setColumnWidth(1, 69);
    setColumnWidth(2, 200);
    setColumnWidth(3, 25);
    setColumnWidth(4, 300);
    setColumnWidth(5, 49);
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

void ProcedureTable::fitToModel() //not working correctly yet
{
    auto rows = model()->rowCount();

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
    constexpr int footerHeight = 10;

    int h = viewport()->height();
    int w = viewport()->width();

    QPainter painter(viewport());

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    painter.fillRect(rect(), Theme::background);

    QPainterPath path = Theme::getHalfCurvedPath(h, w);

    painter.translate(0, h);
    painter.rotate(-90);

    painter.fillPath(path, Theme::sectionBackground);

    painter.end();

    QTableView::paintEvent(e);
   

    painter.begin(viewport());

    painter.translate(0, h);
    painter.rotate(-90);

    QPen borderPen(Theme::border);
    borderPen.setCosmetic(true);
    borderPen.setWidth(2);

    painter.setPen(borderPen);
    painter.drawPath(path);

    painter.resetTransform();

    QPen pen(Theme::fontTurquoise);
    pen.setCosmetic(true);
    pen.setWidth(1);
    painter.setPen(pen);
    
    //drawing the columns:
    if (model() != nullptr && model()->rowCount()) {

        double xPos = 0;

        bool firstVisible(true);

        for (int i = 0; i < model()->columnCount(); i++)
        {

            if (horizontalHeader()->isSectionHidden(i)) {
                continue;
            }

            if (firstVisible) {
                firstVisible = false;
                xPos += horizontalHeader()->sectionSize(i);
                continue;
            }

            painter.drawLine(QPointF(xPos-1, 0), QPointF(xPos-1, h + footerHeight));

            xPos += horizontalHeader()->sectionSize(i);
            
        }

    }

    //drawing the rows:

    if (model() != nullptr && model()->columnCount()) {

        double yPos = 0;

        for (int i = 1; i < model()->rowCount(); i++)
        {

            if (verticalHeader()->isSectionHidden(i)) {
                continue;
            }

            yPos += verticalHeader()->sectionSize(i - 1);


            painter.drawLine(QPointF(1, yPos), QPointF(w-1, yPos));

        }

    }



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

    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Theme::background);

    QPainterPath path = Theme::getHalfCurvedPath(height(), width());;

    painter.translate(width(), 0);
    painter.rotate(90);
    
    painter.fillPath(path, Theme::sectionBackground);

    QPen borderPen(Theme::border);
    borderPen.setWidth(2);
    borderPen.setCosmetic(true);

    painter.setPen(borderPen);
    painter.drawPath(path);

    painter.resetTransform();

    borderPen.setColor(Theme::fontTurquoiseClicked);

    painter.setPen(borderPen);

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
