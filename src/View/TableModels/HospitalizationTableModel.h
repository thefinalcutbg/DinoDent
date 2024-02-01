#pragma once

#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Model/Hospitalization.h"

class HospitalizationTableModel : public QAbstractTableModel
{

	Q_OBJECT

    bool insertRows(int, int, const QModelIndex&) override { return false; }
    bool removeRows(int, int, const QModelIndex&)  override { return false; }
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	std::vector<Hospitalization> m_data;

public:


	HospitalizationTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {};
    void setRows(const std::vector<Hospitalization>& hList);
    int rowCount(const QModelIndex& = QModelIndex())const override { return m_data.size(); };
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 2; }
	~HospitalizationTableModel() {};

	//void removeSelectedRow();
};
