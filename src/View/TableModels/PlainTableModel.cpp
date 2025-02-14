#include "PlainTableModel.h"

#include <QIcon>
#include <QFont>
#include <QPainter>

QIcon getIndicator(const QColor& color) {

    QPixmap px(64, 64);

    px.fill(Qt::transparent);

    QPainter painter(&px);

    painter.setPen(Qt::PenStyle::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QRect(10,10,44,44));
    return QIcon(px);
}

int PlainTableModel::rowCount(const QModelIndex&) const
{
    return m_data.rowCount();
}

int PlainTableModel::columnCount(const QModelIndex&) const
{
    return m_data.columnCount();
}

void PlainTableModel::setTableData(const PlainTable& t)
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

    size_t row = index.row();
    size_t column = index.column();

    if (column == m_data.size() || row == m_data[0].rows.size()) return 0;

    switch (role)
    {
    case Qt::DecorationRole: 

        if (m_data.indicator_column == column && m_data.indicator_row_map.count(row)) {
            return getIndicator(m_data.indicator_row_map.at(row));
        }

        return QIcon(CommonIcon::getPixmap(m_data[column].rows[row].icon));

    case Qt::DisplayRole:
        return m_data[column].rows[row].data.c_str();

    case Qt::TextAlignmentRole:
        switch (m_data[column].alignment)
        {
        case PlainColumn::Left: return int(Qt::AlignLeft | Qt::AlignVCenter);
        case PlainColumn::Center: return int(Qt::AlignCenter | Qt::AlignVCenter);
        case PlainColumn::Right: return int(Qt::AlignRight | Qt::AlignVCenter);
        }
        return QVariant();
    }

    return QVariant();
}
