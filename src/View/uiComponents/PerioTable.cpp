#include "PerioTable.h"
#include <QHeaderView>

PerioTable::PerioTable(QWidget *parent)
	: QTableView(parent)
{
	viewport()->setFocusPolicy(Qt::NoFocus);
	setAttribute(Qt::WA_TransparentForMouseEvents);
}

void PerioTable::initializeSize()
{
	verticalHeader()->hide();
	horizontalHeader()->setFixedHeight(48);

	constexpr int columnsWidth[6]{150, 125, 150, 125, 150, 100};

	for (int i = 0; i < 6; i++) setColumnWidth(i, columnsWidth[i]);

	for (int i = 0; i < 5; i++) setRowHeight(i, 34);
}

PerioTable::~PerioTable()
{
}
