#include "PISDentalActivitiesModel.h"

#include <QIcon>

#include "Model/Dental/ToothUtils.h"

PisDentalActivitiesModel::PisDentalActivitiesModel(QObject* parent) : QAbstractTableModel(parent)
{}


void PisDentalActivitiesModel::setProcedures(
    const std::vector<Procedure>& rows, 
    const std::vector<std::string>& payment
)
{
    beginResetModel();

    this->m_procedures.clear();
    this->m_procedures.reserve(rows.size());

    for (int i = 0; i < rows.size() || i < payment.size(); i++) {

        m_procedures.emplace_back();
        auto& p = m_procedures.back();
        p.code = QString::number(rows[i].code);
        p.tooth = ToothUtils::getToothNumber(rows[i].tooth, rows[i].temp);
        p.date = QString(rows[i].date.toBgStandard().c_str());
        p.name = QString(rows[i].name.c_str());
        p.diagnosis = QString(rows[i].diagnosis.c_str());
        p.payment = payment[i].c_str();

    }

    endResetModel();

    emit dataChanged(index(0, 0), index(m_procedures.size(), poceduresColumnCount));
}


bool PisDentalActivitiesModel::insertRows(int position, int rows, const QModelIndex& index)
{
    return false;
}

bool PisDentalActivitiesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    return false;
}

QVariant PisDentalActivitiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {

            case 0: return "Дата";
            case 1: return "Диагноза";
            case 2: return "Зъб";
            case 3: return "Манипулация";
            case 4: return "Код";
            case 5: return "Статус";

            }
        }
    }

    return QVariant();;
}

int PisDentalActivitiesModel::rowCount(const QModelIndex& parent) const
{
    return m_procedures.size();
}

int PisDentalActivitiesModel::columnCount(const QModelIndex& parent) const
{
    return poceduresColumnCount;
}


QVariant PisDentalActivitiesModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    //if (row == m_procedures.size()) return 0; //why???
    //if (m_procedures.size() == 0) return 0;


    switch (role)
    {

    case Qt::DecorationRole:
        switch (column)
        {
        case 4: return QIcon(":/icons/icon_nzok.png");
        default:
            return QVariant();
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return m_procedures[row].date;
        case 1: return m_procedures[row].diagnosis;
        case 2: return m_procedures[row].tooth != 99 ? m_procedures[row].tooth : QVariant();
        case 3: return m_procedures[row].name;
        case 4: return m_procedures[row].code;
        case 5: return m_procedures[row].payment;

        }
    case Qt::TextAlignmentRole:
        if (column == 0 || column == 2 || column == 4)
            return int(Qt::AlignCenter);
        else {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    return QVariant();

}

PisDentalActivitiesModel::~PisDentalActivitiesModel()
{
}
