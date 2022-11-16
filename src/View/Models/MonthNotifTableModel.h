#pragma once

#include <vector>
#include <QAbstractTableModel>

#include "Model/Financial/MonthNotifRow.h"

struct QMonthNotifRow
{

	QString date;
	FinancialDocType type;
	QString rzi;
	QString uin;
	bool claimed;

	const char* getDocTypeString() const {

		constexpr static const char* typeString[4]{
			"Фактура",
			"Дебитно известие",
			"Кредитно известие",
			"НЕИЗВЕСТНО"
		};

		return typeString[static_cast<int>(type)];
	}
};

class MonthNotifTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<QMonthNotifRow> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	MonthNotifTableModel(QObject* parent = nullptr);

	void setRows(const std::vector<MonthNotifRow>& rows);

	~MonthNotifTableModel();



};

