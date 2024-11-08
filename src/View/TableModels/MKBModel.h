#pragma once
#include <QAbstractTableModel>
#include "Model/ICD10.h"

class MKBModel : public QAbstractTableModel
{
	Q_OBJECT

	const std::vector<ICD10>& mkbList;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	MKBModel(const std::vector<ICD10>& list) : mkbList(list) {}
    int rowCount(const QModelIndex& = QModelIndex()) const override { return mkbList.size(); }
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 2; }
	int getRowFromCode(const std::string& code);
};

