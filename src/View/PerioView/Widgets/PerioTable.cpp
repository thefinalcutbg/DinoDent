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
	horizontalHeader()->setFixedHeight(50);

	constexpr int columnsWidth[6]{100, 100, 200, 100, 200,100};

	for (int i = 0; i < 6; i++) setColumnWidth(i, columnsWidth[i]);

}

PerioTable::~PerioTable()
{
}

/*
namespace IDX { enum row { FMPS, FMBS, BOP }; }
namespace PD { enum row { avg, three, five, seven, above }; }
namespace CAL { enum row { distr, avg, two, four, five }; }
*/
