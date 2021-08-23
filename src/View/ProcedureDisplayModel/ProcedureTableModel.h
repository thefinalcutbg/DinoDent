#pragma once

#include <QAbstractTableModel>

#include "QProcedure.h"


class ProcedureTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QProcedure> procedures;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ProcedureTableModel(QObject* parent = nullptr);

	void setProcedures(const std::vector<Procedure>& p);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	~ProcedureTableModel();

	//void removeSelectedRow();
};
