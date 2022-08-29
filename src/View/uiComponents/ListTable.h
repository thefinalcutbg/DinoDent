#pragma once

#include <QTableView>



class ListTable : public QTableView
{
	Q_OBJECT

	void keyPressEvent(QKeyEvent* event) override;

public:
	ListTable(QWidget* parent);

	~ListTable();

signals:
	void deletePressed();
};

