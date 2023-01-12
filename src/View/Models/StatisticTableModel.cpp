#include "StatisticTableModel.h"


StatisticTableModel::StatisticTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void StatisticTableModel::setRows(std::vector<std::pair<std::string, int>> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto& r : rows)
    {
        this->rows.emplace_back(

            QStatisticModel{
                .name = r.first.c_str(),
                .count = r.second
            }
        );
    }

    endResetModel();

    emit dataChanged(index(0, 0), index(rows.size(), 2));
}

StatisticTableModel::~StatisticTableModel()
{
}

bool StatisticTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool StatisticTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant StatisticTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
            case 0: return "Наименование";
            case 1: return "Брой";
        }

    return QVariant();
}


int StatisticTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int StatisticTableModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

QVariant StatisticTableModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == rows.size()) return 0;
    if (rows.size() == 0) return 0;

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return rows[row].name;
        case 1: return rows[row].count;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        return column == 1 ?
            int(Qt::AlignCenter)
            :
            int(Qt::AlignLeft);
    }

    return QVariant();
}