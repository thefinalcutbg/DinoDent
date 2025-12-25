#pragma once
#include <string>
#include <array>
#include <variant>
#include <optional>

#include "Model/Date.h"
#include "ProcedureCode.h"
#include "Diagnosis.h"
#include "ToothIndex.h"
#include "HISDental.h"

class ToothContainer;
class Tooth;

struct ConstructionRange {

    int toothFrom{ -1 };
    int toothTo{ -1 };

    ConstructionRange(int from, int to) : toothFrom{ from }, toothTo{ to }{
        if (toothTo < toothFrom) std::swap(toothTo, toothFrom);
    }

    int getTeethCount() const { return toothTo - toothFrom + 1; }
    bool isFromSameJaw() const { return (toothFrom < 16) == (toothTo < 16); }
    bool isNotSingleRange() const { return toothFrom != toothTo; }
};

struct RestorationData
{
    std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
    bool post{ false };

    bool isValid() const {
         return std::find(surfaces.begin(), surfaces.end(), true) != surfaces.end();
    }
};

struct AnesthesiaMinutes {
    int minutes{ 0 };
};

typedef std::variant<
            std::monostate, 
            RestorationData,
            AnesthesiaMinutes
            > AdditionalParameters;

typedef std::variant<
    std::monostate,
    ToothIndex,
    ConstructionRange
    > AffectedTeeth;

enum class FinancingSource { NHIF = 2, PHIF = 3, Patient = 4, None = 7, University = 9};

struct Procedure
{
    enum DatabaseSource { Local, PIS, HIS, UnknownSource };

    ProcedureCode code;

    Date date;

    FinancingSource financingSource{ FinancingSource::None };

    Diagnosis diagnosis;

    AffectedTeeth affectedTeeth;

    AdditionalParameters param;

    std::string LPK;
    std::string notes;
    int his_index{ 0 };

    DatabaseSource db_source{ Local };

    int getPriceMultiplier() const;

    double price {0}; //temporary variable - not save anywhere in db

    long long planned_procedure_idx{0};

    //for procedures fetched from his
    std::optional<HISProcedureResult> HIS_fetched_result;

    std::vector<const Tooth*> applyProcedure(ToothContainer& teeth) const;
    //applies the procedures, not taking data into account
    void applyPISProcedure(ToothContainer& teeth) const;
    //be sure AffectedTeeth variant matches the ProcedureScope
    ProcedureScope getScope() const;
    //Can return invalid index with -1 value
    const ToothIndex& getToothIndex() const;
    bool isNhif() const { return financingSource == FinancingSource::NHIF; }
    bool isSentToHis() const { return his_index != 0; };
    bool isHisSupported() const { return code.achiBlock() != 0; }
	bool affectsToothIdx(int toothIdx) const;
    std::vector<int> getArrayIndexes() const;
    std::string getToothString() const;
};

