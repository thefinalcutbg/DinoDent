﻿#include "FinancialTableModel.h"


FinancialTableModel::FinancialTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void FinancialTableModel::setRows(std::vector<FinancialRow> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto& r : rows)
    {
        this->rows.emplace_back(

            QFinancialRow{
                r.number,
                r.nzok,
                QString::fromStdString(r.date.toString()),
                QString::fromStdString(r.recipientId),
                QString::fromStdString(r.recipientName),
                QString::fromStdString(r.recipientPhone)
            }
        );
    }

    endResetModel();
}

FinancialTableModel::~FinancialTableModel()
{
}

bool FinancialTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool FinancialTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant FinancialTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
        case 0: return "ID";
        case 1: return u8"Документ №";
        case 2: return u8"Дата";
        case 3: return u8"ЕГН/ЛНЧ/ЕИК";
        case 4: return u8"Име на получателя";
        case 5: return u8"Телефон";
        }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
    {
        if (section == 4 || section == 5)
            return int(Qt::AlignLeft);
        else
            return int(Qt::AlignCenter);
    }



    return QVariant();
}


int FinancialTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int FinancialTableModel::columnCount(const QModelIndex& parent) const
{
    return 6;
}

#include <QIcon>

QVariant FinancialTableModel::data(const QModelIndex& index, int role) const
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
        case 1:
            if (rows[row].nzok)
                return QIcon(":/icons/icon_nzok.png");
            break;
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return index.row();
        case 1: return rows[row].num;
        case 2: return rows[row].date;
        case 3: return rows[row].recipientId;
        case 4: return rows[row].recipientName;
        case 5: return rows[row].recipientPhone;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 4 || column == 5) return int(Qt::AlignLeft);
        else if (column == 1) return int(Qt::AlignRight);
        else return int(Qt::AlignCenter);
    }

    return QVariant();
}