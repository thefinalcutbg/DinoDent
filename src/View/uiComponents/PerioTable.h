#pragma once

#include <QTableView>
#include "Model/Dental/PerioStatistic.h"

class PerioTable : public QTableView
{
	Q_OBJECT

public:
	PerioTable(QWidget *parent);
	void initializeSize();
	~PerioTable();
};
