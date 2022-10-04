#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/Dental/ReportFile.h"

struct QReportFileRow
{
	QString date;
	QString type;
	QString period;
	QString uin;
	QString status;
	bool error;
};

class ReportFilesModel : public QAbstractTableModel
{
	Q_OBJECT

		std::vector<QReportFileRow> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ReportFilesModel(QObject* parent = nullptr);

	void setRows(const std::vector<ReportFile>& rows);

	~ReportFilesModel();



};
