#pragma once
#include <vector>
#include <QAbstractTableModel>
#include <vector>
#include <string>

class MedicationTableModel : public QAbstractTableModel
{

	std::vector<QString> medList;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent) const override { return medList.size(); }
	int columnCount(const QModelIndex& parent) const override { return 1; }
	QVariant data(const QModelIndex& index, int role) const override;

public:

	void setRows(const std::vector<std::string>& rows);
};