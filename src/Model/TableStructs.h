#pragma once
#include "Model/Date.h"

struct ProcedureRowData
{
    std::string date;
    std::string diagnosis;
    int toothNumber;
    int toothIdx;
    std::string manipulation;
    int code;
    double price;
    bool nzok;
};

struct DetailsSummary
{
    Date date;
    std::string procedureDiagnosis;
    std::string procedureName;
    bool nzok;
    int tooth;
    int code;
    double price{ 0 };
    std::string LPK;
};

struct ProcedureSummary
{
    Date date;
    int code;
    int tooth;
    bool temp;
    bool extr;
};


