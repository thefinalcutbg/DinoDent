#include "ProcedureTemplateModel.h"



ProcedureTemplateModel::ProcedureTemplateModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

#include "View/GlobalFunctions.h"

void ProcedureTemplateModel::setProcedures(std::vector<ProcedureTemplate> procedures)
{
    beginResetModel();


    this->procedures.clear();

    this->procedures.reserve(procedures.size());

    for (auto &m : procedures)
    {
        this->procedures.emplace_back
        (ProcedureRow{ m.code,
            QString::fromStdString(m.name),
            priceToString(m.price), m.nzok });
    }
    endResetModel();

}

ProcedureTemplateModel::~ProcedureTemplateModel()
{
}

bool ProcedureTemplateModel::insertRows(int position, int rows, const QModelIndex& index)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool ProcedureTemplateModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row+count);
    endRemoveRows();

    return true;
}

QVariant ProcedureTemplateModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int ProcedureTemplateModel::rowCount(const QModelIndex& parent) const
{
    return procedures.size();
}

int ProcedureTemplateModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

#include <QIcon>

QVariant ProcedureTemplateModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == procedures.size()) return 0;
    if (procedures.size() == 0) return 0;

    switch (role)
    {

    case Qt::DecorationRole:
        switch (column)
        {
        case code:
            if (procedures[row].nzok)
                return QIcon(":/icons/icon_nzok.png");
            break;
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return index.row();
        case code: return procedures[row].code;
        case name: return procedures[row].name;
        case price: return procedures[row].price;
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 ||column == 3)
            return int(Qt::AlignCenter);
    }

    return QVariant();
}
