#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/Specialty.h"


class SpecialtyTableModel : public QAbstractTableModel
{
	Q_OBJECT

		struct QSpecialty
	{
		int num;
		QString name;
		bool doctor;

	};


	std::vector<QSpecialty> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	SpecialtyTableModel(QObject* parent = nullptr);
	int getRow(const QString& name);
	~SpecialtyTableModel();
};