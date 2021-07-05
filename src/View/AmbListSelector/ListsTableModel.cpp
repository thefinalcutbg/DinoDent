#include "ListsTableModel.h"

ListsTableModel::ListsTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void ListsTableModel::setRows(std::vector<AmbListRow> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto &r : rows)
    {
        this->rows.emplace_back(r);
    }

    endResetModel();
}

ListsTableModel::~ListsTableModel()
{
}

bool ListsTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool ListsTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant ListsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return "ID";
            case 1: return "Номер";
            case 2: return "Дата";
            case 3: return "ЕГН/ЛНЧ";
            case 4: return "Име на пациента";
            }
        }
    }

    return QVariant();
}


int ListsTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int ListsTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}


QVariant ListsTableModel::data(const QModelIndex& index, int role) const
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
        case 0: return index.row();
        case 1: return rows[row].ambNumber;
        case 2: return rows[row].date;
        case 3: return rows[row].patientName;
        case 4: return rows[row].patientID;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 || column == 3)
            return int(Qt::AlignCenter);
    }

    return QVariant();
}