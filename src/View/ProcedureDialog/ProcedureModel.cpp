#include "ProcedureModel.h"

ProcedureModel::ProcedureModel(QObject *parent)
	: QAbstractTableModel(parent)
{
}

void ProcedureModel::setManipulations(std::vector<ManipulationTemplate> manipulations)
{
//    beginResetModel();
   
    
 //   this->manipulations.clear();

    this->manipulations.reserve(manipulations.size());

    for (auto &m : manipulations)
    {
        double intPart;
        auto price = std::modf(m.price, &intPart) == 0.0 ? 
            QString::number(m.price) + " лв." :
            QString::number(m.price, 'f', 2) + " лв.";
            

        this->manipulations.emplace_back
        (ManipulationRow(m.code,
            QString::fromStdString(m.name),
            price));
    }
  //  endResetModel();

}

ProcedureModel::~ProcedureModel()
{
}

bool ProcedureModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool ProcedureModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row+count);
    endRemoveRows();
    
    return true;
}

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return "ID";
            case code: return "Код";
            case name: return "Манипулация";
            case price: return "Цена";
            }
        }
    }
    
	return QVariant();
}

int ProcedureModel::rowCount(const QModelIndex& parent) const
{
    return manipulations.size();
}

int ProcedureModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QVariant ProcedureModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == manipulations.size()) return 0;
    if (manipulations.size() == 0) return 0;

    switch (role)
    {
    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return index.row();
        case code: return manipulations[row].code;
        case name: return manipulations[row].name;
        case price: return manipulations[row].price;
        }
    case Qt::TextAlignmentRole:
        if (column == 1 ||column == 3)
            return int(Qt::AlignCenter);
    }

    return QVariant();
}
