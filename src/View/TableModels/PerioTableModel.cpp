#include "PerioTableModel.h"
#include <QIcon>

PerioTableModel::PerioTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void PerioTableModel::setRows(std::vector<PerioRow> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto &r : rows)
    {
        this->rows.emplace_back(
            
            QPerioListRow{
                QString::fromStdString(r.date.toBgStandard()), 
                QString::fromStdString(r.patientId),
                QString::fromStdString(r.patientName),
                QString::fromStdString(r.patientPhone),
                r.bday
            });
    }

    endResetModel();
}

PerioTableModel::~PerioTableModel()
{
}

bool PerioTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool PerioTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant PerioTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)

            switch (section)
            {
                case 0: return "ID";
                case 1: return "Дата";
                case 2: return "ЕГН/ЛНЧ";
                case 3: return "Име на пациента";
                case 4: return "Телефон";
            }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
    {
        if (section == 3 || section == 4)
            return int(Qt::AlignLeft);
        else
            return int(Qt::AlignCenter);
    }



    return QVariant();
}


int PerioTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int PerioTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}


QVariant PerioTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == rows.size()) return 0;
    if (rows.size() == 0) return 0;

    switch (role)
    {
    case Qt::DecorationRole:
        switch (column)
        {
        case 3:
            return rows[row].bday ? QIcon(":/icons/icon_bday.png") : QVariant();
        default:
            return QVariant();
        }
    case Qt::DisplayRole:
        switch (column){
            case 0: return index.row();
            case 1: return rows[row].date;
            case 2: return rows[row].patientID;
            case 3: return rows[row].patientName;
            case 4: return rows[row].patientPhone;
            default: return QVariant();
        }

    case Qt::TextAlignmentRole:

        return column == 3 || column == 4 ?
            int(Qt::AlignLeft)
            :
            int(Qt::AlignCenter);
    }

    return QVariant();
}