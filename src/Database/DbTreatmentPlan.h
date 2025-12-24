#pragma once
#include "Model/Dental/TreatmentPlan.h"

namespace DbTreatmentPlan{

    //non-const to set planned procedures rowids
    bool insert(TreatmentPlan& t, long long patientRowid);
    bool update(TreatmentPlan& t);
    TreatmentPlan get(long long rowid);
    TreatmentPlan get(long long patientRowId, Date date);
    std::vector<TreatmentPlan::PlannedProcedure> getPendingProcedures(long long patientRowid);
}
