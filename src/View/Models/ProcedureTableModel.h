#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "QProcedure.h"

class ProcedureTableModel : public QAbstractTableModel
{
	Q_OBJECT

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	std::vector<QProcedure> m_procedures;

	static constexpr int poceduresColumnCount = 9;

public:


	ProcedureTableModel(QObject* parent = nullptr);
	void setProcedures(const std::vector<Procedure>& p);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	void filterProcedures(const std::vector<int>& selected);
	~ProcedureTableModel();

	//void removeSelectedRow();
};
