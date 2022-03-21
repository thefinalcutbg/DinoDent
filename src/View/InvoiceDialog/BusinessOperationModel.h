#pragma once
#include <qabstractitemmodel.h>
#include <vector>
#include "Model/Financial/Invoice.h"

struct PrintOperation
{
    QString code, name, price, quantity, total;
};

class BusinessOperationModel : public QAbstractTableModel
{
    std::vector<PrintOperation> m_operations;

public:
    
    BusinessOperationModel(const std::vector<BusinessOperation>& procedures = std::vector<BusinessOperation>{},
        const std::array<bool, 32>& tempTeeth = std::array<bool, 32>{false}
    );

    int rowCount(const QModelIndex& parent) const  override { return m_operations.size(); }
    int columnCount(const QModelIndex& parent) const  override { return 5; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

};