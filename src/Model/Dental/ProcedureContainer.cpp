#include "ProcedureContainer.h"

#include <algorithm>

#include "Model/Dental/ToothContainer.h"

void ProcedureContainer::addProcedures(const std::vector<Procedure>& p)
{

    m_proc.reserve(m_proc.size() + p.size());

    for (size_t i = 0; i < m_proc.size(); i++)
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

    for (size_t i = 0; i < m_proc.size(); i++)
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

bool ProcedureContainer::hasNhifProcedure() const
{
        for (auto& p : m_proc)
            if (p.isNhif())
                return true;

        return false;
}

bool ProcedureContainer::hasDentureManifactureProcedure() const
{
    for (auto& p : m_proc) {
        if ((p.code.nhifCode() == 834 ||
            p.code.nhifCode() == 835)
            &&
            p.financingSource == FinancingSource::NHIF
        ) 
        {
            return true;
        }
    }

    return false;
}

std::string ProcedureContainer::nhifDentureStr() const
{
    bool upper = false;
    bool lower = false;

    for (auto& p : m_proc) {
        
        if (p.financingSource != FinancingSource::NHIF) continue;
        
        if (p.code.nhifCode() == 832) upper = true;

        if (p.code.nhifCode() == 833) lower = true;
    }

    if (upper && lower) return "горна и долна";
    if (upper) return "горна";
    if (lower) return "долна";
    
    return "горна и/или долна";
}

std::string ProcedureContainer::nhifDentureDate() const
{
    Date date;

    for (auto& p : m_proc) {

        if (p.financingSource != FinancingSource::NHIF) continue;

        if (p.code.nhifCode() == 832
         || p.code.nhifCode() == 833
        ) 
        {
            date = std::max(date, p.date);
        }
    }

    if (date.isDefault()) return "";

    return date.toBgStandard();
}

bool ProcedureContainer::hasDentalExam() const
{
    for (auto& p : m_proc) {
        if (p.code.type() == ProcedureType::FullExam) {
            return true;
        }
    }

    return false;
}

void ProcedureContainer::refreshTeethTemporary(const ToothContainer& teeth)
{
    for (auto& procedure : m_proc) {

        auto index = procedure.getToothIndex();

        if (!index.isValid()) continue;

        index.temp = teeth.at(index.index, index.supernumeral)[Dental::Temporary];

        procedure.affectedTeeth = index;
    }
}

void ProcedureContainer::removeProcedure(int idx)
{
    if (idx < 0 || idx >= m_proc.size()) return;

    if (m_proc[idx].his_index) {
        m_removed.push_back(m_proc[idx]);
    }

    m_proc.erase(m_proc.begin() + idx);

    
}

void ProcedureContainer::replaceProcedure(const Procedure& p, int idx)
{
    if (idx < 0 || idx >= m_proc.size()) return;

    if (m_proc[idx].date == p.date)
    {
        m_proc[idx] = p;

        return;
    }

    m_proc.erase(m_proc.begin() + idx);

    addProcedure(p);

}

bool ProcedureContainer::moveProcedure(int from, int to)
{
    int n = static_cast<int>(m_proc.size());

    if (n < 2) return false;

    if (from < 0 || from >= n) return false;
    if (to < 0 || to > n) return false;

    if (from == to || (to == n && from == n - 1)) return false;

    Procedure item = m_proc[from];
    m_proc.erase(m_proc.begin() + from);

    if (to > from) --to;

    m_proc.insert(m_proc.begin() + to, std::move(item));

    std::sort(m_proc.begin(), m_proc.end(),
        [](const Procedure& a, const Procedure& b) {
            return a.date < b.date;
        });

    return true;
}

const Procedure& ProcedureContainer::at(int index) const
{
    return m_proc.at(index);
}

bool ProcedureContainer::hasPregnancy() const
{
    for (auto& p : m_proc) {
        if (p.isNhif() && p.code.nhifCode() == 103 && p.code.ACHICode() == "97017-01") return true;
    }

    return false;
}
