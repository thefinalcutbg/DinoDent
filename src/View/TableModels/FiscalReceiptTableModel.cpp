#include "FiscalReceiptTableModel.h"
#include "AmbListTableModel.h"

FiscalReceiptTableModel::FiscalReceiptTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void FiscalReceiptTableModel::setRows(std::vector<FiscalReceipt> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto& r : rows)
    {
        this->rows.push_back(

            QFiscalReceipt{
                .date = r.datetime.c_str(),
                .fiscal_mem = r.fiscal_memory.c_str(),
                .receipt_num = r.receipt_num.c_str(),
                .amblistNum = r.amblistNumber.c_str(),
            }
        );

        this->rows.back().date[10] = ' ';
    }

    endResetModel();
   
}

bool FiscalReceiptTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool FiscalReceiptTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant FiscalReceiptTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
            case 0: return "Дата";
            case 1: return "Амб.№";
            case 2: return "Фискална памет №";
            case 3: return "Касов бон №";
        }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
    {
        if (section == 5 || section == 6)
            return int(Qt::AlignLeft);
        else
            return int(Qt::AlignCenter);
    }



    return QVariant();
}


int FiscalReceiptTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int FiscalReceiptTableModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QVariant FiscalReceiptTableModel::data(const QModelIndex& index, int role) const
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
            case 0: return rows[row].date;
            case 1: return rows[row].amblistNum;
            case 2: return rows[row].fiscal_mem;
            case 3: return rows[row].receipt_num;
            default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        return int(Qt::AlignVCenter | Qt::AlignHCenter);

    }

    return QVariant();
}


FiscalReceiptTableModel::~FiscalReceiptTableModel()
{
}
