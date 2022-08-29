#include "KSMPModel.h"



QVariant KSMPModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return u8"Код";
                case 1: return u8"Процедура";
                case 2: return u8"Блок";
                case 3: return u8"Наименование";
                case 4: return u8"Глава";
                case 5: return u8"Наименование";
            }
        }
    }

    return QVariant();;
}

QVariant KSMPModel::data(const QModelIndex& index, int role) const
{

    int row = index.row();
    int column = index.column();

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
            case 0: return ksmpList[row]->code.c_str();
            case 1: return ksmpList[row]->name.c_str();
            case 2: return ksmpList[row]->block;
            case 3: return KSMP::blockName(ksmpList[row]->block).c_str();
            case 4: return ksmpList[row]->chapter;
            case 5: return KSMP::chapterName(ksmpList[row]->chapter).c_str();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 || column == 3 || column == 5)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else
            return int(Qt::AlignCenter);
    }

    return QVariant();
}

int KSMPModel::getRowFromCode(const std::string& code)
{
    for (int i = 0; i < ksmpList.size(); i++)
    {
        if (code == ksmpList[i]->code) {
            return i;
        }
    }

    return -1;
}
