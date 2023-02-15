#include "AmbListTableModel.h"

AmbListTableModel::AmbListTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void AmbListTableModel::setRows(std::vector<AmbRow> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto &r : rows)
    {
        this->rows.emplace_back(

            QAmbListRow{
                QString::fromStdString(r.number),
                r.his,
                r.nhif,
                QString::fromStdString(r.date.toBgStandard()),
                QString::fromStdString(r.patientId),
                QString::fromStdString(r.patientName),
                QString::fromStdString(r.patientPhone)
            }
        );
    }

    endResetModel();
}

AmbListTableModel::~AmbListTableModel()
{
}

bool AmbListTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool AmbListTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant AmbListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            switch (section)
            {
                case 0: return "ID";
                case 1: return "НЗОК";
                case 2: return "Амб.№";
                case 3: return "Дата";
                case 4: return "ЕГН/ЛНЧ";
                case 5: return "Име на пациента";
                case 6: return "Телефон";
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


int AmbListTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int AmbListTableModel::columnCount(const QModelIndex& parent) const
{
    return 7;
}

#include <QIcon>

QVariant AmbListTableModel::data(const QModelIndex& index, int role) const
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
            if (rows[row].nhif) return QIcon(":/icons/icon_nzok.png");
            break;
        case 2:
            if (rows[row].his) return QIcon(":/icons/icon_his.png");
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return index.row();
        case 2: return rows[row].ambNumber;
        case 3: return rows[row].date;
        case 4: return rows[row].patientID;
        case 5: return rows[row].patientName;
        case 6: return rows[row].patientPhone;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 5 || column == 6 || column == 2) return int(Qt::AlignLeft);
        else return int(Qt::AlignCenter);
    }

    return QVariant();
}