#pragma once
#include "Model/Dental/TreatmentPlan.h"
#include <set>

namespace DbTreatmentPlan{

    //non-const to set planned procedures rowids
    bool insert(TreatmentPlan& t, long long patientRowid);
    bool update(TreatmentPlan& t);
    TreatmentPlan get(long long rowid);
    long long getExistingPlan(long long patientRowId);
    std::vector<Procedure> getPendingProcedures(long long patientRowid, const std::set<long long>& exclude = {});
}
