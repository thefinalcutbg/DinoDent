#pragma once
#include <string>
#include <array>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include "Model/Date.h"
#include "StatusData.h"
#include "ProcedureCode.h"
#include "Diagnosis.h"

class ToothContainer;

struct NoData {};

struct ConstructionRange {

    int tooth_begin{ 0 };
    int tooth_end{ 0 };
};

struct ProcedureObtData
{
    std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
    bool post;
};

struct AnesthesiaMinutes {
    int minutes{ 0 };
};

typedef std::variant<
            NoData, 
            ProcedureObtData, 
            ConstructionRange,
            AnesthesiaMinutes
        > Result;

enum class FinancingSource { NHIF = 2, PHIF = 3, Patient = 4 };


struct Procedure
{
    ProcedureCode code;

    Date date;

    FinancingSource financingSource{ FinancingSource::Patient };

    Diagnosis diagnosis;

    int tooth{ -1 };        //-1 for general/several teeth, any in range 0-31 for specific tooth
    bool temp{ false };
    bool hyperdontic{ false };

    Result result{ NoData{} };

    std::string LPK;
    std::string notes;

    int his_index{ 0 };

    void applyProcedure(ToothContainer& teeth) const;
    //applies the procedures, not taking data into account
    void applyPISProcedure(ToothContainer& teeth) const;

    bool isToothSpecific() const;
    bool isRangeSpecific() const {
        return 
            code.type() == ProcedureType::bridge || 
            code.type() == ProcedureType::fibersplint ||
            code.type() == ProcedureType::denture;
    }
    bool isNhif() const {
        return financingSource == FinancingSource::NHIF;
    }

    bool isSentToHis() const { return his_index != 0; };
};

struct ProcedureTemplate {
    ProcedureCode code;
    bool nhif;
};



