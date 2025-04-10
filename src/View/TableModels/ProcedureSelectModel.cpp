﻿#include "ProcedureSelectModel.h"
#include <QIcon>


void ProcedureSelectModel::selectFinancingSource(FinancingSource source)
{
    beginResetModel();

    for (int i = 0; i < m_procedures.size(); i++) {

        m_selectedRows[i] = (m_procedures[i].fsource == source);
    }

    endResetModel();

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void ProcedureSelectModel::selectAll()
{
    beginResetModel();

    for (int i = 0; i < m_selectedRows.size(); i++)
    {
        m_selectedRows[i] = true;
    }

    endResetModel();

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));

}

ProcedureSelectModel::ProcedureSelectModel(const std::vector<Procedure>& procedures, QObject* parent) : QAbstractTableModel(parent)
{
    this->m_procedures.reserve(procedures.size());
    this->m_selectedRows.reserve(procedures.size());

    for (auto& p : procedures){
        m_procedures.push_back(QProcedure{ p });
        m_selectedRows.emplace_back(true);
    }
}

QVariant ProcedureSelectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "Дата";
                case 1: return "Диагноза";
                case 2: return "Зъб";
                case 3: return "Процедура";
                case 4: return "Код";
      //          case 5: return "Цена";
            }
        }
    }

    if (role == Qt::DecorationRole && orientation == Qt::Horizontal && section == 0)
    {
        return QIcon(":/icons/icon_print.png");
    }

    return QVariant();;
}

int ProcedureSelectModel::rowCount(const QModelIndex&) const
{
    return m_procedures.size();
}

int ProcedureSelectModel::columnCount(const QModelIndex&) const
{
    return 5;
}

std::vector<int> ProcedureSelectModel::selectedRows()
{
    std::vector<int> result;

    for (int i = 0; i < m_selectedRows.size(); i++)
    {
        if (m_selectedRows[i])
            result.push_back(i);
    }

    return result;
}


QVariant ProcedureSelectModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid()) return QVariant();

        int row = index.row();
        int column = index.column();

        //if (row == m_procedures.size()) return 0; //why???
        //if (m_procedures.size() == 0) return 0;


        switch (role)
        {

        case Qt::DecorationRole:
            switch (column)
            {
            case 0:
                switch (m_procedures[row].fsource)
                {
                    case FinancingSource::NHIF: return QIcon(":/icons/icon_nhif.png");
                    case FinancingSource::PHIF: return QIcon(":/icons/icon_phif.png");
                    case FinancingSource::Patient: return QIcon(":/icons/icon_user.png");
                    default: return QIcon();
                }
                    
                return QVariant();
            default:
                return QVariant();
            }

        case Qt::DisplayRole:
            switch (column)
            {
               case 0: return m_procedures[row].date;
               case 1: return m_procedures[row].diagnosis;
               case 2: return m_procedures[row].tooth;
               case 3: return m_procedures[row].procedureName;
               case 4: return m_procedures[row].ACHICode;
    //           case 5: return m_procedures[row].price;
            }
        case Qt::TextAlignmentRole:
            if (column == 2 || column == 4 || column == 5)
                return int(Qt::AlignHCenter | Qt::AlignVCenter);
            else if(column == 0 )
                return int(Qt::AlignRight | Qt::AlignVCenter);
            else
                return int(Qt::AlignLeft | Qt::AlignVCenter);

        case Qt::CheckStateRole:

            if (column == 0)
            {
                return m_selectedRows[row] ?
                    Qt::Checked
                    :
                    Qt::Unchecked;

            }
        }

        return QVariant();

}
bool ProcedureSelectModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();

    if (!index.isValid())
        return false;
    if (role == Qt::CheckStateRole)
    {
        if ((Qt::CheckState)value.toInt() == Qt::Checked)
        {
            m_selectedRows[row] = true;
            return true;
        }
        else
        {
            m_selectedRows[row] = false;
            return true;
        }
    }
    return false;
}

Qt::ItemFlags ProcedureSelectModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    if (index.column() == 0)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }

    return Qt::ItemIsEnabled;
}

ProcedureSelectModel::~ProcedureSelectModel()
{
}
