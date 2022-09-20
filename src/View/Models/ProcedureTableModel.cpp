#include "ProcedureTableModel.h"
#include <QIcon>

ProcedureTableModel::ProcedureTableModel(QObject* parent) : QAbstractTableModel(parent)
{}


void ProcedureTableModel::setProcedures(const std::vector<Procedure>& rows)
{
    beginResetModel();

    this->m_procedures.clear();
    this->m_procedures.reserve(rows.size());

    for (auto& p : rows)
        this->m_procedures.emplace_back(QProcedure{p});

    endResetModel();

    emit dataChanged(index(0, 0), index(m_procedures.size(), poceduresColumnCount));
}


bool ProcedureTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    return false;
}

bool ProcedureTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    return false;
}

QVariant ProcedureTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return u8"ID";
            case 1: return u8"Дата";
            case 2: return u8"Диагноза";
            case 3: return u8"Зъб";
            case 4: return u8"Манипулация";
            case 5: return u8"Код";
            case 6: return u8"КСМП";
            case 7: return u8"Цена";
            case 8: return u8"Извършил";
            }
        }
    }

    return QVariant();;
}

int ProcedureTableModel::rowCount(const QModelIndex& parent) const
{
    return m_procedures.size();
}

int ProcedureTableModel::columnCount(const QModelIndex& parent) const
{
    return poceduresColumnCount;
}

void ProcedureTableModel::filterProcedures(const std::vector<int>& selected)
{
    std::vector<QProcedure> filtered;

    filtered.reserve(m_procedures.size());

    for (int row : selected){
        filtered.push_back(m_procedures[row]);
    }

    m_procedures = filtered;

    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_procedures.size(), 7);
    emit dataChanged(topLeft, bottomRight);
}



QVariant ProcedureTableModel::data(const QModelIndex& index, int role) const
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
            case 5:
                if (m_procedures[row].nhif)
                    return QIcon(":/icons/icon_nzok.png");
            default:
                return QVariant();
            }

        case Qt::DisplayRole:
            switch (column)
            {
               case 0: return index.row();
               case 1: return m_procedures[row].date;
               case 2: return m_procedures[row].diagnosis;
               case 3: return m_procedures[row].tooth != 99 ? m_procedures[row].tooth : QVariant();
               case 4: return m_procedures[row].procedureName;
               case 5: return m_procedures[row].code;
               case 6: return m_procedures[row].ksmp;
               case 7: return m_procedures[row].price;
               case 8: return m_procedures[row].doctor;
            }
        case Qt::TextAlignmentRole:
             if (column == 1 || column == 3 || column == 5 || column == 6 || column == 7)
                return int(Qt::AlignCenter);
        }

        return QVariant();

}

ProcedureTableModel::~ProcedureTableModel()
{
}
