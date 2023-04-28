#pragma once

#include <QAbstractTableModel>
#include "Model/Dental/ProcedureCode.h"

class ProcedureCodeModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<ProcedureCode> m_list;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ProcedureCodeModel();
	int rowCount(const QModelIndex& parent = QModelIndex()) const override { return m_list.size(); }
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 2; }
};
