#include "PrescriptionTableModel.h"

PrescriptionTableModel::PrescriptionTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void PrescriptionTableModel::setRows(std::vector<PrescriptionRow> rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto& r : rows)
    {
        this->rows.emplace_back(

            QPerscrModel{
                .isSentToHis = !r.nrn.empty(),
                .date = QString::fromStdString(r.date.toBgStandard()),
                .nrn = QString::fromStdString(r.nrn),
                .patientID = QString::fromStdString(r.patientId),
                .patientName = QString::fromStdString(r.patientName),
                .patientPhone = QString::fromStdString(r.patientPhone),
                .bday = r.bday
            }
        );
    }

    endResetModel();
}

PrescriptionTableModel::~PrescriptionTableModel()
{
}

bool PrescriptionTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool PrescriptionTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant PrescriptionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
        case 0: return "ID";
        case 1: return "Дата";
        case 2: return "НРН";
        case 3: return "ЕГН/ЛНЧ/ИН";
        case 4: return "Име на пациента";
        case 5: return "Телефон";
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


int PrescriptionTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int PrescriptionTableModel::columnCount(const QModelIndex& parent) const
{
    return 6;
}

#include <QIcon>

QVariant PrescriptionTableModel::data(const QModelIndex& index, int role) const
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
          case 2:
              return rows[row].isSentToHis ? QIcon(":/icons/icon_his.png") : QVariant();
          case 3:
              return rows[row].bday ? QIcon(":/icons/icon_bday.png") : QVariant();
          default:
              return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return index.row();
        case 1: return rows[row].date;
        case 2: return rows[row].nrn;
        case 3: return rows[row].patientID;
        case 4: return rows[row].patientName;
        case 5: return rows[row].patientPhone;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 || column == 3) return int(Qt::AlignCenter);
        else return int(Qt::AlignVCenter | Qt::AlignLeft);
    }

    return QVariant();
}