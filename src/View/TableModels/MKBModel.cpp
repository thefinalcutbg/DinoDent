#include "MKBModel.h"

QVariant MKBModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant MKBModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int column = index.column();

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return mkbList[row].code().c_str();
        case 1: return mkbList[row].name().c_str();

        }
    case Qt::TextAlignmentRole:
            return column == 1 ? 
                int(Qt::AlignLeft | Qt::AlignVCenter) 
                : 
                int(Qt::AlignCenter);
    }

    return QVariant();
}

int MKBModel::getRowFromCode(const std::string& code)
{
    for (int i = 0; i < mkbList.size(); i++)
    {
        if (code == mkbList[i].code()) {
            return i;
        }
    }

    return -1;
}
