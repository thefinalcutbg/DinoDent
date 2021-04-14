#pragma once
#include "Model/Manipulation/ManipulationTemplate.h"
#include <QAbstractTableModel>

#include <vector>

enum procedureHeader { code = 1, name, price };

struct ManipulationRow
{
	ManipulationRow(int code, QString name, QString price) 
		: code{ code }, name{ name }, price{ price } 
	{
		
	};

	int code;
	QString name;
	QString price;
};

class ProcedureModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<ManipulationRow> manipulations;

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


public:
	ProcedureModel(QObject *parent = nullptr);

	void setManipulations(std::vector<ManipulationTemplate> manipulations);

	~ProcedureModel();


	//void removeSelectedRow();
};
