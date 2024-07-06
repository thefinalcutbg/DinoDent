#pragma once
#include <string>
#include <array>
#include <variant>
#include "Model/Date.h"
#include "ProcedureCode.h"
#include "Diagnosis.h"
#include "ToothIndex.h"

class ToothContainer;

struct ConstructionRange {

    int tooth_begin{ -1 };
    int tooth_end{ -1 };
};

struct RestorationData
{
    std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
    bool post{ false };
};

struct AnesthesiaMinutes {
    int minutes{ 0 };
};

typedef std::variant<
            std::monostate, 
            RestorationData, 
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

    ToothIndex tooth_idx;

    Result result{ std::monostate{} };

    std::string LPK;
    std::string notes;

    int his_index{ 0 };

    enum DatabaseSource { Local, PIS, HIS };
    DatabaseSource db_source{ Local };

    void applyProcedure(ToothContainer& teeth) const;
    //applies the procedures, not taking data into account
    void applyPISProcedure(ToothContainer& teeth) const;

    bool isNhif() const {
        return financingSource == FinancingSource::NHIF;
    }

    bool isSentToHis() const { return his_index != 0; };

    bool isHisSupported() const {
        return code.hisType() != 0;
    }

    std::string getToothString() const;
};

struct ProcedureTemplate {
    ProcedureCode code;
    bool nhif;
};



