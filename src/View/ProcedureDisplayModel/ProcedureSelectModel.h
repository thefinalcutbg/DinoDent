#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "QProcedure.h"

class ProcedureSelectModel : public QAbstractTableModel
{
	Q_OBJECT

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	std::vector<QProcedure> m_procedures;
	std::vector<bool> m_selectedRows;

public:


	ProcedureSelectModel(const std::vector<Procedure>& p, QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	std::vector<int> selectedRows();
	~ProcedureSelectModel();

	//void removeSelectedRow();
};
