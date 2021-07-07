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

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(20);
    //setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWordWrap(true);
}

void ListTable::setDimensions()
{
    hideColumn(0);
    setColumnWidth(1, 50);
    setColumnWidth(2, 80);
    setColumnWidth(3, 80);
    setColumnWidth(4, 250);

}

void ListTable::resizeEvent(QResizeEvent* ev)
{
    setColumnWidth(4, width() - 50 - 80 - 80 - 2);

    QTableView::resizeEvent(ev);
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