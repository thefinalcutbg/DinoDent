#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>
#include "ui_TableViewDialog.h"

class TableViewDialog : public QDialog
{
	Q_OBJECT

	int m_selectedRow{ -1 };
	int m_column;

	QSortFilterProxyModel m_proxyModel;
	QAbstractTableModel& m_model;

public:
	TableViewDialog(QAbstractTableModel& model, int column, const std::string& code = std::string(), QWidget* parent = nullptr);
	std::string getResult();
	~TableViewDialog();

private:
	Ui::TableViewDialogClass ui;
};
