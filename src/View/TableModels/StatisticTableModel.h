#pragma once

#include <QAbstractTableModel>

class StatisticTableModel : public QAbstractTableModel
{
	Q_OBJECT


	struct QStatisticModel
	{
		QString name;
		int count;
	};


	std::vector<QStatisticModel> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	StatisticTableModel(QObject* parent = nullptr);

	void setRows(std::vector<std::pair<std::string, int>> rows);

	~StatisticTableModel();



};