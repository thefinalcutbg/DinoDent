#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/TableRows.h"

struct QPerioListRow
{
	QString date;
	QString patientID;
	QString patientName;
	QString patientPhone;
};

class PerioTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QPerioListRow> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	PerioTableModel(QObject* parent = nullptr);

	void setRows(std::vector<PerioRow> rows);

	~PerioTableModel();



};

