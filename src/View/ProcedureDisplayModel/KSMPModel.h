#pragma once
#include <QAbstractTableModel>
#include "Model/KSMP.h"

class KSMPModel : public QAbstractTableModel
{
	Q_OBJECT

	KsmpList& ksmpList;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	KSMPModel(KsmpList& list) : ksmpList(list){}
	int rowCount(const QModelIndex& parent = QModelIndex()) const override {return ksmpList.size();}
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 6; }
	int getRowFromCode(const std::string& code);
};

