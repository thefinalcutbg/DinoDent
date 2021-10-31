#pragma once

#include <QAbstractTableModel>
#include "Model/PerioStatistic.h"

#include <string_view>

class StatisticTableModel : public QAbstractTableModel
{
	Q_OBJECT

	QString m_IDX[5];
	QString m_PDData[5];
	QString m_CALData[5];

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	StatisticTableModel(QObject* parent = nullptr);

	void setStatistics(const PerioStatistic& stat);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	~StatisticTableModel();

	//void removeSelectedRow();
};