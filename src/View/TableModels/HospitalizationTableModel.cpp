#include "HospitalizationTableModel.h"
#include <QIcon>
#include <QFont>

void HospitalizationTableModel::setRows(const std::vector<Hospitalization>& rows)
{
    beginResetModel();

    m_data.clear();

    for (int i = rows.size() - 1; i != -1; i--) m_data.push_back(rows[i]);

    endResetModel();

    emit dataChanged(index(0, 0), index(rows.size(), columnCount()));
}


QVariant HospitalizationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return "Дата на създаване";
            case 1: return "Статус";
            }
        }
    }

    if (role == Qt::TextAlignmentRole) {
        return section == 0 ? int(Qt::AlignCenter) : int(Qt::AlignLeft);
    }

    return QVariant();;
}


QVariant HospitalizationTableModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    //if (row == m_procedures.size()) return 0; //why???
    //if (m_procedures.size() == 0) return 0;

    switch (role)
    {

    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return QString::fromStdString(m_data[row].authoredOn.toBgStandard());
        
        case 1: 
            switch (m_data[row].status) 
            {
                case Hospitalization::Active: return "Активна";
                case Hospitalization::Terminated: return "Прекратена";
                case Hospitalization::Completed: return "Завършила";
                case Hospitalization::Planned: return "Планирана";
                default: break;
            }
        }
    case Qt::FontRole:
        if (column == 1) {
            QFont font;
            font.setBold(true);
            return font;
        }
        else break;
    case Qt::ForegroundRole:
    {
        if (column == 1) {

            switch (m_data[row].status)
            {
                case Hospitalization::Active: return QColor(Qt::red);
                case Hospitalization::Terminated: return  QColor(Qt::darkGreen);
                case Hospitalization::Completed: return QColor(Qt::darkGreen);
                case Hospitalization::Planned: return QColor(Qt::darkYellow);
                default: break;
            }
        }
        else break;

    }
    case Qt::TextAlignmentRole:
        return column == 0 ? int(Qt::AlignCenter) : int(Qt::AlignLeft);
    }

    return QVariant();

}
