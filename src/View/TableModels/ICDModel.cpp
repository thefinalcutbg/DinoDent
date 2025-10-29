#include "ICDModel.h"

QVariant ICDModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "Код";
                case 1: return "Наименование";
            }
        }
    }

    return QVariant();;
}

QVariant ICDModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int column = index.column();

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
            case 0: return icd_list[row].code().c_str();
            case 1: return icd_list[row].name().c_str();
            default: break;
        }
    case Qt::TextAlignmentRole:
            return column == 1 ? 
                int(Qt::AlignLeft | Qt::AlignVCenter) 
                : 
                int(Qt::AlignCenter);
    }

    return QVariant();
}

int ICDModel::getRowFromCode(const std::string& code)
{
    for (int i = 0; i < icd_list.size(); i++)
    {
        if (code == icd_list[i].code()) {
            return i;
        }
    }

    return -1;
}
