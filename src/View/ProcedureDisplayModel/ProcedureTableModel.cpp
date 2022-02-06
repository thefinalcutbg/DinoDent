#include "ProcedureTableModel.h"
#include <QIcon>

ProcedureTableModel::ProcedureTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}


void ProcedureTableModel::setProcedures(const std::vector<Procedure>& rows)
{
    beginResetModel();

    this->procedures.clear();
    this->procedures.reserve(rows.size());

    for (auto& p : rows)
        this->procedures.emplace_back(QProcedure{p});

    endResetModel();
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
            case 0: return "ID";
            case 1: return "Дата";
            case 2: return "Диагноза";
            case 3: return "Зъб";
            case 4: return "Манипулация";
            case 5: return "Код";
            case 6: return "Цена";
            case 7: return "Извършил";
            }
        }
    }

    return QVariant();;
}

int ProcedureTableModel::rowCount(const QModelIndex& parent) const
{
    return procedures.size();
}

int ProcedureTableModel::columnCount(const QModelIndex& parent) const
{
    return 8;
}

QVariant ProcedureTableModel::data(const QModelIndex& index, int role) const
{

        
        if (!index.isValid()) return QVariant();

        int row = index.row();
        int column = index.column();

        if (row == procedures.size()) return 0; //why???
        if (procedures.size() == 0) return 0;


        switch (role)
        {

        case Qt::DecorationRole:
            switch (column)
            {
            case 5:
                if (procedures[row].nzok)
                    return QIcon(":/icons/icon_nzok.png");
            default:
                return QVariant();
            }

        case Qt::DisplayRole:
            switch (column)
            {
               case 0: return index.row();
               case 1: return procedures[row].date;
               case 2: return procedures[row].diagnosis;
               case 3: return procedures[row].tooth != 99 ? procedures[row].tooth : QVariant();
               case 4: return procedures[row].procedureName;
               case 5: return procedures[row].code;
               case 6: return procedures[row].price;
               case 7: return procedures[row].doctor;
            }
        case Qt::TextAlignmentRole:
             if (column == 1 || column == 3 || column == 5 || column == 6)
                return int(Qt::AlignCenter);
        }

        return QVariant();

}

ProcedureTableModel::~ProcedureTableModel()
{
}
