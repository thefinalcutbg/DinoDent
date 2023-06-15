#pragma once
#include <QAbstractTableModel>
#include "Model/Financial/FiscalReceipt.h"

struct QFiscalReceipt
{
	QString date;
	QString fiscal_mem;
	QString receipt_num;
	QString amblistNum;
	QString patientId;

};

class FiscalReceiptTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	FiscalReceiptTableModel(QObject* parent = nullptr);

	void setRows(std::vector<FiscalReceipt> rows);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	~FiscalReceiptTableModel();

private:

	std::vector<QFiscalReceipt> rows;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


};
