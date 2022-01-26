#include "ListTable.h"

#include <QHeaderView>
#include <QScrollBar>
#include <QKeyEvent>



ListTable::ListTable(QWidget* parent)
    : QTableView(parent)
{

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    horizontalHeader()->setHighlightSections(false);
    
    verticalHeader()->setVisible(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewport()->setFocusPolicy(Qt::StrongFocus);

    setShowGrid(true);


    verticalHeader()->setDefaultSectionSize(20);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

}

/*
void ListTable::resizeEvent(QResizeEvent* ev)
{
    int lastColumnIdx = model()->columnCount() - 1;
    int columnWidth{ 0 };

    for (int i = 0; i < model()->columnCount()-1; i++)
    {
        columnWidth += this->columnWidth(i);
    }
    setColumnWidth(lastColumnIdx, width() - columnWidth - 2);

    QTableView::resizeEvent(ev);
}
*/


void ListTable::keyPressEvent(QKeyEvent* event)
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

ListTable::~ListTable()
{
}