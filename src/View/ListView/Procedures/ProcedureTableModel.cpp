#include "ProcedureTableModel.h"
#include <cmath>

ProcedureTableModel::ProcedureTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}

void ProcedureTableModel::setProcedure(const std::vector<RowData>& rows)
{
    beginResetModel();

    this->manipulations.clear();
    this->manipulations.reserve(rows.size());

    for (RowData row : rows)
    {
        double dummy; //passing 0 as second variable not always works, hense the dummy
        auto price = std::modf(row.price, &dummy) == 0.0 ?
            QString::number(row.price) + " лв." :
            QString::number(row.price, 'f', 2) + " лв.";

        this->manipulations.emplace_back(


            QRow
            {
                QString::fromStdString(row.date),
                QString::fromStdString(row.diagnosis),
                row.toothNumber,
                QString::fromStdString(row.manipulation),
                QString::number(row.code),
                price,
                row.nzok
            }
        );
    }

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
            case 7: return "";
            }
        }
    }

    return QVariant();;
}

int ProcedureTableModel::rowCount(const QModelIndex& parent) const
{
    return manipulations.size();
}

int ProcedureTableModel::columnCount(const QModelIndex& parent) const
{
    return 7;
}

QVariant ProcedureTableModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid()) return QVariant();

        int row = index.row();
        int column = index.column();

        if (row == manipulations.size()) return 0;
        if (manipulations.size() == 0) return 0;


        switch (role)
        {

        case Qt::DecorationRole:
            switch (column)
            {
            case 5:
                if (manipulations[row].nzok)
                    return QIcon(QPixmap("nzok.png"));
                break;
            default:
                return QVariant();
            }

        case Qt::DisplayRole:
            switch (column)
            {
               case 0: return index.row();
               case 1: return manipulations[row].date;
               case 2: return manipulations[row].diagnosis;
               case 3: return (manipulations[row].tooth != -1) ? manipulations[row].tooth : QVariant();
               case 4: return manipulations[row].manipulation;
               case 5: return manipulations[row].code;
               case 6: return manipulations[row].price;
            }
        case Qt::TextAlignmentRole:
            //if (column == 1 || column == 3 || column == 5 || column == 6)
                return int(Qt::AlignCenter);



        }



        return QVariant();

}

ProcedureTableModel::~ProcedureTableModel()
{
}
