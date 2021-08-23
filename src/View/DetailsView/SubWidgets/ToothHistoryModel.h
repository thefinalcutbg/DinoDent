#pragma once

#include <QAbstractTableModel>
#include "Model/Procedure/TableStructs.h"

struct QHistory
{
	QString date;
	QString diagnosis;
	QString procedure_name;
	int code;
	QString price;
	QString doctor;
	bool nzok;
};

class ToothHistoryModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QHistory> m_procedures;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) { return false; }
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) { return false; }
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ToothHistoryModel(QObject* parent = nullptr);

	void setProcedure(const std::vector<DetailsSummary>& manipulations);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override { return m_procedures.size(); }
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 6; };
	~ToothHistoryModel();
};

