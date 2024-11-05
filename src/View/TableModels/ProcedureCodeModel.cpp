#include "ProcedureCodeModel.h"


ProcedureCodeModel::ProcedureCodeModel() :
    m_list(ProcedureCode::getProcedures())
{}

QVariant ProcedureCodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return "Код";
            case 1: return "Процедура";
            }
        }
    }

    return QVariant();;
}

QVariant ProcedureCodeModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int column = index.column();

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
            case 0: return m_list[row].code().c_str();
            case 1: return m_list[row].name().c_str();
            default: break;
        }
    case Qt::TextAlignmentRole:
        if (column == 1)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else
            return int(Qt::AlignCenter);
    }

    return QVariant();
}
