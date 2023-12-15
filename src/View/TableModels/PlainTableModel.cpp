#include "PlainTableModel.h"
#include <algorithm>

#include <QIcon>

int PlainTableModel::rowCount(const QModelIndex& parent) const
{
    return m_data.rowCount();
}

int PlainTableModel::columnCount(const QModelIndex& parent) const
{
    return m_data.columnCount();
}

void PlainTableModel::setData(const PlainTable& t)
{
    beginResetModel();
    m_data = t;
    endResetModel();
}

QVariant PlainTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_data[section].name.c_str();

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
          return int(Qt::AlignCenter);

    return QVariant();
}


QVariant PlainTableModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (column == m_data.size() || row == m_data[0].rows.size()) return 0;

    switch (role)
    {
    case Qt::DecorationRole:
        switch (m_data[column].rows[row].icon)
        {
        case PlainCell::NHIF: return QIcon(":/icons/icon_nhif.png");
        case PlainCell::HIS: return QIcon(":/icons/icon_his.png");
        case PlainCell::PHIF: return QIcon(":/icons/icon_phif.png");
        case PlainCell::BDAY: return QIcon(":/icons/icon_bday.png");
        default: return QVariant();
        }

    case Qt::DisplayRole:
        return m_data[column].rows[row].data.c_str();

    case Qt::TextAlignmentRole:
        switch (m_data[column].alignment)
        {
        case PlainColumn::Left: return int(Qt::AlignLeft | Qt::AlignVCenter);
        case PlainColumn::Center: return int(Qt::AlignCenter | Qt::AlignVCenter);
        case PlainColumn::Right: return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return QVariant();
}