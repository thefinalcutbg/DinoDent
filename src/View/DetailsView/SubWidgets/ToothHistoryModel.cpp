#include "ToothHistoryModel.h"
#include <qicon.h>
#include "View/GlobalFunctions.h"

ToothHistoryModel::ToothHistoryModel(QObject* parent) : QAbstractTableModel(parent)
{
}

QVariant ToothHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return u8"Дата";
            case 1: return u8"Диагноза";
            case 2: return u8"Манипулация";
            case 3: return u8"Код";
            case 4: return u8"Цена";
            case 5: return u8"Извършил:";
            }
        }
    }

    return QVariant();;
}

QVariant ToothHistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == m_procedures.size()) return 0;
    if (m_procedures.size() == 0) return 0;


    switch (role)
    {

    case Qt::DecorationRole:
        switch (column)
        {
        case 3:
            if (m_procedures[row].nzok)
                return QIcon(QPixmap("nzok.png"));
        default:
            return QVariant{};
        }

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return m_procedures[row].date;
        case 1: return m_procedures[row].diagnosis;
        case 2: return m_procedures[row].procedure_name;
        case 3: return m_procedures[row].code;
        case 4: return m_procedures[row].price;
        case 5: return m_procedures[row].doctor;
        }
    case Qt::TextAlignmentRole:
        if (column == 0 || column == 3 || column == 4)
        return int(Qt::AlignCenter);
    }

    return QVariant();
}



void ToothHistoryModel::setProcedure(const std::vector<DetailsSummary>& procedures)
{
    m_procedures.reserve(procedures.size());

    for (auto& p : procedures)
        m_procedures.emplace_back(QHistory
            {
                        QString::fromStdString(Date::toString(p.date)),
                        QString::fromStdString(p.procedureDiagnosis),
                        QString::fromStdString(p.procedureName),
                        p.code,
                        priceToString(p.price),
                        QString::fromStdString(p.LPK),
                        p.nzok
            });
}

ToothHistoryModel::~ToothHistoryModel()
{
}
