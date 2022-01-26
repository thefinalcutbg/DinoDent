#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/TableRows.h"

struct QPatientRow
{
	QString patientID;
	QString patientName;
	QString patientPhone;
};

class PatientTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QPatientRow> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	PatientTableModel(QObject* parent = nullptr);

	void setRows(const std::vector<PatientRow>& rows);

	~PatientTableModel();



};

