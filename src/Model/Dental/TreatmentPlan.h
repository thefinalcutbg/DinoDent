#pragma once

#include "Model/Dental/ToothContainer.h"
#include "Model/Date.h"
#include "Model/Dental/Procedure.h"

struct TreatmentPlan
{
    long long rowid {0};
    Date date {Date::currentDate()};
    std::string LPK; // made by;
    ToothContainer teeth;
    bool is_completed {false};

    struct PlannedProcedure
    {
        PlannedProcedure(const Procedure& p);

        PlannedProcedure() {};

        Procedure getProcedure() const;

        std::string getName() const;

        std::string getPriceLabel();

        long long rowid = 0;
        ProcedureCode code;
        std::string name; //if empty take procedure code description
        Diagnosis diagnosis;
        std::string notes;
        AffectedTeeth affectedTeeth;
        AdditionalParameters param;
        std::pair<double, double> priceRange {0,0};
        bool isCompleted {false}; //dynamic
    };

    struct Stage{

        std::string notes;
        std::vector<PlannedProcedure> plannedProcedures;
        std::pair<double, double> getPriceRange() const;
    };

    std::vector<Stage> stages;

    bool lastStageIsConclusion = false;
};
