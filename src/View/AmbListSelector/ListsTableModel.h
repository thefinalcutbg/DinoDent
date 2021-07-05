#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/AmbListRow.h"

struct QAmbListRow
{
	int ambNumber;
	QString date;
	QString patientName;
	QString patientID;

	QAmbListRow(AmbListRow r)
		:
		ambNumber(r.ambNumber),
		date(QString::fromStdString(Date::toString(r.date))),
		patientName(QString::fromStdString(r.patientName)),
		patientID(QString::fromStdString(r.patientId)) {};
};

class ListsTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QAmbListRow> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ListsTableModel(QObject* parent = nullptr);

	void setRows(std::vector<AmbListRow> rows);

	~ListsTableModel();



};

