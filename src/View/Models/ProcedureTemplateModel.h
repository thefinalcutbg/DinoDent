#pragma once
#include "Model/Dental/ProcedureTemplate.h"
#include <QAbstractTableModel>

#include <vector>

enum procedureHeader { code = 1, name, price };

struct ProcedureRow
{
	int code;
	QString name;
	double price;
	bool nzok;
};

class ProcedureTemplateModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<ProcedureRow> procedures;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


public:
	ProcedureTemplateModel(QObject *parent = nullptr);

	void setProcedures(std::vector<ProcedureTemplate> procedures);
	const ProcedureRow& getProcedureRow(int index) const;
	~ProcedureTemplateModel();


	//void removeSelectedRow();
};
