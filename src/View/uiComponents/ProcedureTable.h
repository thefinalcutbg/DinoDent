#pragma once
#include <QTableView>
#include <QHeaderView>
#include <thread>

class ProcedureHeader : public QHeaderView
{
	void paintEvent(QPaintEvent* e) override;

public:
	ProcedureHeader(Qt::Orientation orientation, QWidget* parent = nullptr)
		:
		QHeaderView(orientation, parent){}
};


class ProcedureTable : public QTableView
{
	Q_OBJECT

 void keyPressEvent(QKeyEvent* event) override;

	void paintEvent(QPaintEvent* e) override;
	 
	ProcedureHeader header;



	void focusOutEvent(QFocusEvent* event) override {

		//setCurrentIndex(QModelIndex());
	}

	void fitToModel(int rows);

public:
	ProcedureTable(QWidget* parent);
	
	~ProcedureTable();

	int selectedRow();
	void setAmbListLayout();
	void setProcedureHistoryLayout();
	void setProcedurePrintSelectLayout();
	void setBusinessOperationLayout();
	//buggy
	void setPISActivitiesLayout();
	



signals:
	void deletePressed();


};