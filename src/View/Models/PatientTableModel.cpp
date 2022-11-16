#include "PatientTableModel.h"

PatientTableModel::PatientTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void PatientTableModel::setRows(const std::vector<PatientRow>& rows)
{
    beginResetModel();

    this->rows.clear();
    this->rows.reserve(rows.size());

    for (auto &r : rows)
    {
        this->rows.emplace_back(

            QPatientRow{
                QString::fromStdString(r.patientId),
                QString::fromStdString(r.name),
                QString::fromStdString(r.phone)
            }
        );
    }

    endResetModel();
}

PatientTableModel::~PatientTableModel()
{
}

bool PatientTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool PatientTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant PatientTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
            switch (section)
            {
                case 0: return "ID";
                case 1: return "ЕГН/ЛНЧ";
                case 2: return "Име на пациента";
                case 3: return "Телефон";
            }
    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
    {
        if (section == 2 || section == 3)
            return int(Qt::AlignLeft);
        else
            return int(Qt::AlignCenter);
    }

    return QVariant();
}


int PatientTableModel::rowCount(const QModelIndex& parent) const
{
    return rows.size();
}

int PatientTableModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}


QVariant PatientTableModel::data(const QModelIndex& index, int role) const
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
        case 1: return rows[row].patientID;
        case 2: return rows[row].patientName;
        case 3: return rows[row].patientPhone;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        return column == 2 || column == 3 ?
            int(Qt::AlignLeft)
            :
            int(Qt::AlignCenter);
    }

    return QVariant();
}