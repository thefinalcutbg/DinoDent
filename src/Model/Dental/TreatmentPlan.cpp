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

std::string TreatmentPlan::PlannedProcedure::getName() const
{
    return name.empty() ? code.name() : name;
}

std::pair<double, double> TreatmentPlan::Stage::getPriceLabel() const
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
