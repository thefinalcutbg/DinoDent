#pragma once
#include "Model/Dental/TreatmentPlan.h"
#include <set>

namespace DbTreatmentPlan{

    //non-const to set planned procedures rowids
    bool insert(TreatmentPlan& t, long long patientRowid);
    bool update(TreatmentPlan& t, const std::vector<long long>& deletedProcedures);
    TreatmentPlan get(long long rowid);
    long long getActiveTreatmentPlan(long long patientRowId);
    std::vector<Procedure> getPendingProcedures(long long patientRowid, const std::set<long long>& exclude = {});
    std::set<long long> getCompletedProcedures(long long patientRowid);
    std::pair<double, double> getPlannedProcedurePrice(long long rowid);
    std::vector<TreatmentPlan::PlannedProcedure> getProcedureSummary(long long planRowid);
    bool setAsCompleted(long long rowid);
}
