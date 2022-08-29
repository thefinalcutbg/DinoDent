#include "MonthNotifTableModel.h"

MonthNotifTableModel::MonthNotifTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void MonthNotifTableModel::setRows(const std::vector<MonthNotifRow>& rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto &r : rows)
    {
        this->rows.emplace_back(

            QMonthNotifRow{
                r.date.toBgStandard().data(),
                r.type,
                r.rziCode.data(),
                r.uin.data(),
                r.claimed
            }
        );
    }

    endResetModel();
}

MonthNotifTableModel::~MonthNotifTableModel()
{
}

bool MonthNotifTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool MonthNotifTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant MonthNotifTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            switch (section)
            {
                case 0: return "Дата";
                case 1: return "Месечно известие";
                case 2: return "Практика";
                case 3: return "УИН";
                case 4: return "Подаден";
            }


    return QVariant();
}


int MonthNotifTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int MonthNotifTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

#include <QIcon>

QVariant MonthNotifTableModel::data(const QModelIndex& index, int role) const
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
        case 4:
            if (rows[row].claimed)
                return QIcon(":/icons/icon_apply.png");
            break;
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return rows[row].date;
        case 1: return rows[row].getDocTypeString();
        case 2: return rows[row].rzi;
        case 3: return rows[row].uin;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
       
        if (column == 1) return int(Qt::AlignLeft);
        else return int(Qt::AlignCenter);
    }

    return QVariant();
}