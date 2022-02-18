#include "BusinessOperationModel.h"
#include "View/GlobalFunctions.h"

BusinessOperationModel::BusinessOperationModel(const std::vector<BusinessOperation>& operations, const std::array<bool, 32>& tempTeeth)
{
    m_operations.reserve(operations.size());

    for (auto& o : operations)
    {
        m_operations.emplace_back(PrintOperation
        {
            QString::fromStdString(o.activity_code),
            QString::fromStdString(o.activity_name),
            priceToString(o.unit_price),
            QString::number(o.quantity),
            priceToString(o.value_price)
        });
    }

}

QVariant BusinessOperationModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (role != Qt::DisplayRole) return QVariant();

    switch (col)
    {
        case 0: return m_operations[row].code;
        case 1: return m_operations[row].name;
        case 2: return m_operations[row].price;
        case 3: return m_operations[row].quantity;
        case 4: return m_operations[row].total;
    }

    return QVariant();
}

