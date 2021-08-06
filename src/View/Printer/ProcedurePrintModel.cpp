#include "ProcedurePrintModel.h"
#include "Model/Tooth/ToothUtils.h"

ProcedurePrintModel::ProcedurePrintModel(const std::vector<Procedure>& procedures, const std::array<bool, 32>& tempTeeth)
{
    constexpr int maxProcedures = 8;

    m_procedures.reserve(maxProcedures);

    for (int i = 0; i < std::min(maxProcedures, static_cast<int>(procedures.size())); i++)
    {
        auto& p = procedures[i];

        m_procedures.emplace_back(PrintProcedure
        {
            QString::fromStdString(Date::toString(p.date)),
            QString::fromStdString(p.diagnosis),
            QString::fromStdString(ToothUtils::getNomenclature(p.tooth, tempTeeth[p.tooth])),
            QString::fromStdString(p.name),
            QString::number(p.code)
        });
    }
}

QVariant ProcedurePrintModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (role != Qt::DisplayRole) return QVariant();

    switch (col)
    {
        case 0: return m_procedures[row].date;
        case 1: return m_procedures[row].diagnosis;
        case 2: return m_procedures[row].tooth;
        case 3: return m_procedures[row].proc;
        case 4: return m_procedures[row].code;
        case 5: return m_procedures[row].dur;
    }

    return QVariant();
}

