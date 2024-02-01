#include "SpecialtyTableModel.h"
#include "Model/Specialty.h"

SpecialtyTableModel::SpecialtyTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    for (auto& s : Specialty::names())
    {
        rows.push_back(QSpecialty
            {
                .num = s.second,
                .name = s.first.c_str(),
                .doctor = Specialty::isDoctor(s.second)
            }
        );
    }

    auto compare = [&](QSpecialty& first, QSpecialty& second)
    {
        return first.num < second.num;
    };

    std::sort(rows.begin(), rows.end(), compare);
}

int SpecialtyTableModel::getRow(const QString& name)
{
    for (int i = 0; i < rows.size(); i++) {
        if (rows[i].name == name) return i;
    }

    return -1;
}

SpecialtyTableModel::~SpecialtyTableModel()
{
}

bool SpecialtyTableModel::insertRows(int position, int, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), position, position);
    endInsertRows();
    return true;
}

bool SpecialtyTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();

    return true;
}


QVariant SpecialtyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        switch (section)
        {
        case 0: return "Код";
        case 1: return "Наименование";
        case 2: return "Категория";
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


int SpecialtyTableModel::rowCount(const QModelIndex&) const
{
    return rows.size();
}

int SpecialtyTableModel::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant SpecialtyTableModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) return QVariant();

    size_t row = index.row();
    int column = index.column();

    if (row == rows.size()) return 0;
    if (rows.size() == 0) return 0;

   // static const char* doc{ "Медицина" };
   // static const char* dentist{ "Дентална медицина" };

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return rows[row].num;
        case 1: return rows[row].name;
       // case 2: return rows[row].doctor ? doc : dentist;

        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
            return int(Qt::AlignVCenter | Qt::AlignLeft);
    }

    return QVariant();
}
