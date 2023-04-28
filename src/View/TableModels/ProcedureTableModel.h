#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "QProcedure.h"

class ProcedureTableModel : public QAbstractTableModel
{
	Q_OBJECT

	int m_destinationDropRow{ -1 };

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	std::vector<QProcedure> m_procedures;

	Qt::ItemFlags flags(const QModelIndex& index) const override;
	Qt::DropActions supportedDropActions() const override;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& index) override;

	static constexpr int poceduresColumnCount = 9;

public:


	ProcedureTableModel(QObject* parent = nullptr);
	void setProcedures(const std::vector<Procedure>& p);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	void filterProcedures(const std::vector<int>& selected);
	~ProcedureTableModel();


	int lastDroppedRowIndex() const { return m_destinationDropRow; }



	//void removeSelectedRow();
};
