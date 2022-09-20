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
    horizontalHeader()->setStretchLastSection(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

}


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