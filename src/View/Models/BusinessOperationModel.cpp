#include "BusinessOperationModel.h"
#include "View/GlobalFunctions.h"
#include "Model/Financial/BusinessOperation.h"

QVariant BusinessOperationModel::headerData(int section, Qt::Orientation orientation, int role) const
{   
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return u8"№";
            case 1: return u8"Код";
            case 2: return u8"Наименование на стоката/услугата";
            case 3: return u8"Кол-во";
            case 4: return u8"Ед.цена";
            case 5: return u8"Сума";
            }
        }
    }
    
    return QAbstractItemModel::headerData(section, orientation, role);
}

BusinessOperationModel::BusinessOperationModel(const std::vector<BusinessOperation>& operations)
{
    m_operations.reserve(operations.size());

    for (auto& o : operations)
    {
        m_operations.emplace_back(PrintOperation
        {
            QString::fromStdString(o.activity_code),
            QString::fromStdString(o.activity_name),
            QString::number(o.quantity),
            formatDoubleWithDecimal(o.unit_price),
            formatDoubleWithDecimal(o.value_price)
        });
    }

}

void BusinessOperationModel::setBusinessOperations(const BusinessOperations& businessOp)
{
    m_operations.clear();
    m_operations.reserve(businessOp.size());


    beginResetModel();

    for (auto& o : businessOp)
    {
        m_operations.emplace_back(PrintOperation
            {
                QString::fromStdString(o.activity_code),
                QString::fromStdString(o.activity_name),
                QString::number(o.quantity),
                formatDoubleWithDecimal(o.unit_price),
                formatDoubleWithDecimal(o.value_price)
            });
    }

    endResetModel();

    emit dataChanged(index(0,0), index(m_operations.size(), 7));
}

QVariant BusinessOperationModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int column = index.column();

   if (role == Qt::DisplayRole) {

        switch (column){

            case 0: return row + 1;
            case 1: return m_operations[row].code;
            case 2: return m_operations[row].name;
            case 3: return m_operations[row].quantity + " бр.";
            case 4: return m_operations[row].price;
            case 5: return m_operations[row].total;

        }

    }
    else if (role == Qt::TextAlignmentRole) {

        return column == 2 ?
            int(Qt::AlignLeft | Qt::AlignVCenter)
            :
            int(Qt::AlignCenter);
    }
   



    return QVariant();
}

