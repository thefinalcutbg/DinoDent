#pragma once

#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Model/Hospitalization.h"

class HospitalizationTableModel : public QAbstractTableModel
{

	Q_OBJECT

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) { return false; }
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) { return false; }
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	std::vector<Hospitalization> m_data;

public:


	HospitalizationTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {};
	void setData(const std::vector<Hospitalization>& hList);
	int rowCount(const QModelIndex& parent = QModelIndex())const override { return m_data.size(); };
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 2; }
	~HospitalizationTableModel() {};

	//void removeSelectedRow();
};
