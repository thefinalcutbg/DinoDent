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

    struct PlannedProcedure
    {
        PlannedProcedure(const Procedure& p);

        Procedure getProcedure() const;

        std::string getName() const;

        std::string getPriceLabel();

        ProcedureCode code;
        std::string name; //if empty take procedure code description
        Diagnosis diagnosis;
        std::string notes;
        AffectedTeeth affectedTeeth;
        AdditionalParameters param;
        int scenario = 0;
        std::pair<double, double> priceRange {0,0};
    };

    struct Stage{

        std::string notes;
        std::vector<PlannedProcedure> plannedProcedures;
        std::pair<double, double> getPriceLabel() const;
    };

    std::vector<Stage> stages;

    std::string disclamer; //auto generation?
};
