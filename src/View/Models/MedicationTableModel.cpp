#include "MedicationTableModel.h"

void MedicationTableModel::setRows(const std::vector<std::string>& rows)
{
    beginResetModel();

    this->medList.clear();
    this->medList.reserve(rows.size());

    for (auto& r : rows) this->medList.emplace_back(r.c_str());
    
    endResetModel();

    emit dataChanged(index(0, 0), index(medList.size(), 1));
}


QVariant MedicationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
         case 0: return "Изписани лекарства: ";
        }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal)
    {
        return int(Qt::AlignVCenter | Qt::AlignHCenter);
    }

    return QVariant();
}

QVariant MedicationTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == medList.size()) return 0;
    if (!medList.size()) return 0;

    if(role == Qt::DisplayRole) return medList[row];

    if (role == Qt::TextAlignmentRole) return int(Qt::AlignVCenter | Qt::AlignHCenter);

    return QVariant();
}

