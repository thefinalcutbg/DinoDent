#pragma once
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QKeyEvent>

class ProcedureTable : public QTableView
{
	Q_OBJECT

 virtual void keyPressEvent(QKeyEvent* event) override;

public:
	ProcedureTable(QWidget* parent);
	
	~ProcedureTable();
	void setDimensions();
signals:
	void deletePressed();


};

