#pragma once

#include <QAbstractTableModel>

#include "Model/PlainTable.h"

class PlainTableModel : public QAbstractTableModel
{
	Q_OBJECT

	PlainTable m_data;

    bool insertRows(int, int, const QModelIndex& = QModelIndex()) override { return false; }
    bool removeRows(int, int, const QModelIndex& = QModelIndex()) override { return false; }
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
    PlainTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {};

    void setTableData(const PlainTable& t);

	~PlainTableModel() {};
};
