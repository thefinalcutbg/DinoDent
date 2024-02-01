#include "ProcedureTemplateModel.h"



ProcedureTemplateModel::ProcedureTemplateModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

#include "View/GlobalFunctions.h"

void ProcedureTemplateModel::setProcedures(std::vector<ProcedureListElement> procedures)
{
    beginResetModel();


    this->procedures.clear();

    this->procedures.reserve(procedures.size());

    for (auto &m : procedures)
    {
        this->procedures.emplace_back
        (
            ProcedureRow{ 
            .code = QString::number(m.code.oldCode()),
            .name = m.code.name().c_str(),
            .nhif = m.nhif 
            }
        );
    }
    endResetModel();

}

const ProcedureRow& ProcedureTemplateModel::getProcedureRow(int index) const
{
    return procedures[index];
}

ProcedureTemplateModel::~ProcedureTemplateModel()
{
}

bool ProcedureTemplateModel::insertRows(int position, int, const QModelIndex&)
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
            }
        }
    }

    return QVariant();
}

int ProcedureTemplateModel::rowCount(const QModelIndex&) const
{
    return procedures.size();
}

int ProcedureTemplateModel::columnCount(const QModelIndex&) const
{
    return 3;
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
            if (procedures[row].nhif)
                return QIcon(":/icons/icon_nhif.png");
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
            default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 ||column == 3)
            return int(Qt::AlignCenter);
    }

    return QVariant();
}
