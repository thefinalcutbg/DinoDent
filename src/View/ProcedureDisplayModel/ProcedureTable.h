#pragma once
#include <QTableView>


class ProcedureTable : public QTableView
{
	Q_OBJECT

 void keyPressEvent(QKeyEvent* event) override;

public:
	ProcedureTable(QWidget* parent);
	
	~ProcedureTable();

	void setAmbListLayout();
	void setProcedureHistoryLayout();
	void setProcedureSelectLayout();

signals:
	void deletePressed();


};

