#pragma once
#include <qabstractitemmodel.h>
#include <vector>
#include "Model/Financial/BusinessOperation.h"

struct PrintOperation
{
    QString code, name, quantity, price,  total;
};

class BusinessOperationModel : public QAbstractTableModel
{
    std::vector<PrintOperation> m_operations;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public:
    
    BusinessOperationModel(const std::vector<BusinessOperation>& businessOp = std::vector<BusinessOperation>{});

    void setBusinessOperations(const BusinessOperations& businessOp);
    int rowCount(const QModelIndex&) const  override { return m_operations.size(); }
    int columnCount(const QModelIndex&) const  override { return 6; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

};
