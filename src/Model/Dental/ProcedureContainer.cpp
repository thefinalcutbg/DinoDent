#include "ProcedureContainer.h"
#include "Model/Dental/ToothContainer.h"

void ProcedureContainer::addProcedures(const std::vector<Procedure>& p)
{

    m_proc.reserve(m_proc.size() + p.size());

    for (int i = 0; i < m_proc.size(); i++)
    {
        if (p[0].date < m_proc[i].date)
        {
            m_proc.insert(m_proc.begin() + i, p.begin(), p.end());
            return;
        }
    }

    //if list is empty, or date is max
    for (auto& mInsert : p)
    {
        m_proc.push_back(mInsert);
    }
}

void ProcedureContainer::addProcedure(const Procedure& p)
{
    m_proc.reserve(m_proc.size() + 1);

    for (int i = 0; i < m_proc.size(); i++)
    {
        if (p.date < m_proc[i].date)
        {
            m_proc.insert(m_proc.begin() + i, p);
            return;
        }
    }

    //if list is empty, or date is max

        m_proc.push_back(p);

}

bool ProcedureContainer::hasNzokProcedure() const
{
        for (auto& p : m_proc)
            if (p.nzok)
                return true;

        return false;
}

void ProcedureContainer::refreshTeethTemporary(const ToothContainer& teeth)
{
    for (auto& procedure : m_proc) {
        if (procedure.tooth < 0 || procedure.tooth > 31) continue;
        procedure.temp = teeth.at(procedure.tooth).temporary.exists();
    }
}

void ProcedureContainer::removeProcedure(int idx)
{
    m_proc.erase(m_proc.begin() + idx);
}

void ProcedureContainer::replaceProcedure(const Procedure& p, int idx)
{
    if (m_proc[idx].date == p.date)
    {
        m_proc[idx] = p;
    }
    else
    {
        removeProcedure(idx);
        addProcedure(p);
    }
}

const Procedure& ProcedureContainer::at(int index) const
{
    return m_proc.at(index);
}

bool ProcedureContainer::hasPregnancy() const
{
    for (auto& p : m_proc) {
        if (p.nzok && p.code == 101 && p.ksmp == "97017-01") return true;
    }

    return false;
}