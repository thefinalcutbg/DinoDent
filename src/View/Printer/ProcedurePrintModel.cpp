#include "ProcedurePrintModel.h"
#include "Model/Tooth/ToothUtils.h"


PrintProcedure::PrintProcedure(const Procedure& procedure)
    :
    date{ QString::fromStdString(procedure.date.toString()) },
    diagnosis{ QString::fromStdString(procedure.diagnosis) },
    tooth{ QString::fromStdString(ToothUtils::getNomenclature(procedure.tooth, procedure.temp)) },
    proc{ QString::fromStdString(procedure.name) },
    code{ QString::number(procedure.code) }
{}


ProcedurePrintModel::ProcedurePrintModel(const std::vector<Procedure>& procedures, const std::vector<int>& selectedProcedures)
{
    constexpr int maxProcedures = 6;

    m_procedures.reserve(maxProcedures);

    for (auto row : selectedProcedures)
    {
        m_procedures.emplace_back(procedures[row]);
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
