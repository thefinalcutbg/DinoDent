#include "ReportFilesModel.h"


ReportFilesModel::ReportFilesModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void ReportFilesModel::setRows(const std::vector<ReportFile>& rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto& r : rows)
    {
        this->rows.emplace_back(

            QReportFileRow{
                r.uploaded_on.c_str(),
                r.type.c_str(),
                r.period.c_str(),
                r.lpk.c_str(),
                r.status.c_str(),
                !r.error.empty()
            }
        );
    }

    endResetModel();


    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));

    
}

ReportFilesModel::~ReportFilesModel()
{
}

bool ReportFilesModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool ReportFilesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant ReportFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
            case 0: return u8"Дата";
            case 1: return u8"Документ";
            case 2: return u8"Статус";
            case 3: return u8"Период";
            case 4: return u8"Доктор";

        }


    return QVariant();
}


int ReportFilesModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int ReportFilesModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

#include <QIcon>

QVariant ReportFilesModel::data(const QModelIndex& index, int role) const
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
        case 0:
            if (rows[row].error) return QIcon(":/icons/icon_remove.png");

            if (rows[row].status == u8"успешно обработен") return QIcon(":/icons/icon_apply.png");
            break;
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return rows[row].date;
        case 1: return rows[row].type;
        case 2: return rows[row].status;
        case 3: return rows[row].period;
        case 4: return rows[row].uin;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:

        if (column == 0 || column == 3) return  int(Qt::AlignCenter);
        else return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();
}