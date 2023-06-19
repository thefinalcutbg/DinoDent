#pragma once
#include "Model/Date.h"
#include "Model/Dental/ToothIndex.h"

struct ProcedureRowData
{
    std::string date;
    std::string diagnosis;
    int toothNumber;
    int toothIdx;
    std::string manipulation;
    int code;
    double price;
    bool nhif;
};

struct DetailsSummary
{
    Date date;
    std::string procedureDiagnosis;
    std::string procedureName;
    bool nhif;
    int tooth;
    int code;
    double price{ 0 };
    std::string LPK;
};

struct ProcedureSummary
{
    Date date;
    int code;
    ToothIndex tooth_idx;
    bool extr;

};


