#pragma once
#include <QAbstractTableModel>
#include "Model/ICD10.h"

class ICDModel : public QAbstractTableModel
{
	Q_OBJECT

	const std::vector<ICD10>& icd_list;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	ICDModel(const std::vector<ICD10>& list) : icd_list(list) {}
    int rowCount(const QModelIndex& = QModelIndex()) const override { return icd_list.size(); }
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 2; }
	int getRowFromCode(const std::string& code);
};