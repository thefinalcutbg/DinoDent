#pragma once

#include <QAbstractTableModel>

#include "Model/PlainTable.h"

class PlainTableModel : public QAbstractTableModel
{
	Q_OBJECT

	PlainTable m_data;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) { return false; }
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) { return false; }
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	PlainTableModel(QObject* parent = nullptr) {};

	void setData(const PlainTable& t);

	~PlainTableModel() {};
};