#include "ProcedureTableModel.h"
#include <QIcon>
#include <QRegularExpression>
ProcedureTableModel::ProcedureTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}


void ProcedureTableModel::setProcedures(const std::vector<Procedure>& rows, bool separateLongNotes)
{
    beginResetModel();

    this->m_procedures.clear();
    this->m_procedures.reserve(rows.size());

    for (auto& p : rows){
        this->m_procedures.emplace_back(QProcedure{p});

        constexpr int chunk = 1000;

        if (!separateLongNotes || p.notes.size() <= chunk) continue;

        //LOGIC FOR SPLITTING THE LONG NOTES WHEN PRINTING

        QString fullText = m_procedures.back().notes;

        const int total = fullText.size();

        int pos = 0;
        int boundary = fullText.lastIndexOf(QRegularExpression("\\s+"), pos + chunk);

        if (boundary <= pos) boundary = pos + chunk;

        m_procedures.back().notes = fullText.mid(pos, boundary - pos).trimmed();

        pos = boundary;

        while (pos < total && fullText.at(pos).isSpace()) ++pos;

        while (pos < total) {

            if (total - pos <= chunk) {
                m_procedures.emplace_back(QProcedure{ fullText.mid(pos).trimmed() });
                break;
            }

            // Find break position
            int nextBoundary = fullText.lastIndexOf(QRegularExpression("\\s+"), pos + chunk);

            if (nextBoundary <= pos) nextBoundary = pos + chunk;

            QString segment = fullText.mid(pos, nextBoundary - pos).trimmed();

            m_procedures.emplace_back(QProcedure{ segment });

            pos = nextBoundary;

            while (pos < total && fullText.at(pos).isSpace()) ++pos;
        }

    }
    endResetModel();

    emit dataChanged(index(0, 0), index(m_procedures.size(), proceduresColumnCount));
}


bool ProcedureTableModel::insertRows(int, int, const QModelIndex&)
{
    return false;
}

bool ProcedureTableModel::removeRows(int, int, const QModelIndex&)
{
    return false;
}

QVariant ProcedureTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "ID";
                case 1: return "Дата";
                case 2: return "Диагноза";
                case 3: return "Зъб";
                case 4: return "Процедура";
                case 5: return "НЗОК";
                case 6: return "КСМП";
                case 7: return "Извършил";
                case 8: return "Бележки";
            }
        }
    }

    return QVariant();;
}

int ProcedureTableModel::rowCount(const QModelIndex&) const
{
    return m_procedures.size();
}

int ProcedureTableModel::columnCount(const QModelIndex&) const
{
    return proceduresColumnCount;
}

void ProcedureTableModel::filterProcedures(const std::vector<int>& selected)
{
    std::vector<QProcedure> filtered;

    filtered.reserve(m_procedures.size());

    for (int row : selected){
        filtered.push_back(m_procedures[row]);
    }

    m_procedures = filtered;

    QModelIndex topLeft = createIndex(0, 0);
    QModelIndex bottomRight = createIndex(m_procedures.size(), 7);
    emit dataChanged(topLeft, bottomRight);
}



QVariant ProcedureTableModel::data(const QModelIndex& index, int role) const
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
            case 6:
                switch (m_procedures[row].fsource)
                {
                    case FinancingSource::NHIF: return QIcon(":/icons/icon_nhif.png");
                    case FinancingSource::PHIF: return QIcon(":/icons/icon_phif.png");
                    case FinancingSource::Patient: return QIcon(":/icons/icon_user.png");
                    case FinancingSource::University: return QIcon(":/icons/icon_uni.png");
                    default: return QIcon();
                }
                break;
            case 1:
                switch (m_procedures[row].db_source)
                {
                    case Procedure::Local: return QIcon(":/icons/icon_db.png");
                    case Procedure::PIS: return QIcon(":/icons/icon_nhif.png");
                    case Procedure::HIS:  return QIcon(":/icons/icon_his.png");
                }
                break;
            default:
                return QVariant();
            }

        case Qt::DisplayRole:
            switch (column)
            {
               case 0: return index.row();
               case 1: return m_procedures[row].date;
               case 2: return m_procedures[row].diagnosis;
               case 3: return m_procedures[row].tooth;
               case 4: return m_procedures[row].procedureName;
               case 5: return m_procedures[row].nhifCode;
               case 6: return m_procedures[row].ACHICode;
               case 7: return m_procedures[row].doctor;
               case 8: return m_procedures[row].notes;
               default: break;
            }
        case Qt::TextAlignmentRole:
             if (column == 1 || column == 3 || column == 5 || column == 6 || column == 7)
                return int(Qt::AlignCenter);
        }

        return QVariant();

}

ProcedureTableModel::~ProcedureTableModel()
{
}

Qt::ItemFlags ProcedureTableModel::flags(const QModelIndex&) const
{
    auto flags = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    return flags;
}

Qt::DropActions ProcedureTableModel::supportedDropActions() const
{
    auto flags = Qt::DropActions();
    flags |= Qt::MoveAction;
    return flags;
}


bool ProcedureTableModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& index)
{

    Q_UNUSED(column)
        QModelIndex newIndex;
    if (index.isValid()) // row was dropped directly on an item (parent)
    {
        // If we apply dropMimeData without any modifications,
        // the data overwrites the given row.
        // However, we would like to insert the data *after* the given row.
        // The TableModel inserts a row if the parent is the hidden root parent
        // (provided by QModelIndex()), so we use that.
        newIndex = QModelIndex();
        row = index.row() + 1;
    }
    else
        newIndex = index;

    if (row == -1)
        row = rowCount();

    m_destinationDropRow = row;

    return QAbstractTableModel::dropMimeData(data, action, row, 0, newIndex);
}
