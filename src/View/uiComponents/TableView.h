#pragma once
#include <QTableView>
#include <QHeaderView>
#include <thread>

class TableViewHeader : public QHeaderView
{
	void paintEvent(QPaintEvent* e) override;

	

public:
	TableViewHeader(Qt::Orientation orientation, QWidget* parent = nullptr)
		:
		QHeaderView(orientation, parent){}
};


class TableView : public QTableView
{
	Q_OBJECT

	int m_dropRow{ 0 };

	void keyPressEvent(QKeyEvent* event) override;

	void paintEvent(QPaintEvent* e) override;
	 
	TableViewHeader header;


	void focusOutEvent(QFocusEvent* event) override {

		//setCurrentIndex(QModelIndex());
	}

	QMenu* menu{ nullptr };

public:
	TableView(QWidget* parent);
	
	~TableView();

	int selectedRow();
	Q_INVOKABLE int selectedRow() const;
	void dropEvent(QDropEvent* e);

	void setAmbListLayout();
	void setProcedureHistoryLayout();
	void setProcedurePrintSelectLayout();
	void setBusinessOperationLayout();
	void setMedicationLayot();
	void setStatisticLayout();
	//buggy
	void setPISActivitiesLayout();

	void fitToModel();

	void enableContextMenu(bool enabled);

signals:
	void deletePressed(int row);
	void rowDragged();
	void editPressed(int row);

};
