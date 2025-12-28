#include "TreatmentPlan.h"
#include "Model/User.h"

TreatmentPlan::PlannedProcedure::PlannedProcedure(const Procedure &p)
    :
    code(p.code),
    diagnosis(p.diagnosis),
    notes(p.notes),
    affectedTeeth(p.affectedTeeth),
    param(p.param)
{}

Procedure TreatmentPlan::PlannedProcedure::getProcedure() const
{
    return Procedure{
        .code = code,
        .date = Date::currentDate(),
        .diagnosis = diagnosis,
        .affectedTeeth = affectedTeeth,
        .param = param,
        .LPK = User::doctor().LPK
    };
}

const std::string& TreatmentPlan::PlannedProcedure::getNameText() const
{
    return name.empty() ? code.name() : name;
}

const std::string &TreatmentPlan::PlannedProcedure::getDiagnosisText() const
{
    return diagnosis.additional_descr.size() ? diagnosis.additional_descr : diagnosis.icd.name();
}

std::pair<double, double> TreatmentPlan::Stage::getPriceRange() const
{
    double priceMin = 0;
    double priceMax = 0;

    for(auto& p : plannedProcedures)
    {
        priceMin += p.priceRange.first;
        priceMax += p.priceRange.second;
    }

    return std::make_pair(priceMin, priceMax);
}

std::pair<double, double> TreatmentPlan::getTotalPrice() const
{
    double priceMin = 0;
    double priceMax = 0;

    for(auto& s :stages){
        priceMin += s.getPriceRange().first;
        priceMax += s.getPriceRange().second;
    }

    return std::make_pair(priceMin, priceMax);
}
