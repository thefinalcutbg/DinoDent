#pragma once
#include <string>
#include <array>
#include <variant>
#include <optional>
#include "Model/Date.h"
#include "ProcedureTemplate.h"
#include "StatusData.h"

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

struct Anesthesia {
    int minutes{ 0 };
};

typedef std::variant<
            NoData, 
            ProcedureObtData, 
            ConstructionRange, 
            Anesthesia
        > Result;

enum class ProcedureType 
{ 
	general = 0, 
	any = 1, 
	obturation = 2, 
	extraction = 3, 
	endo = 4, 
	crown = 5, 
	implant = 6, 
	bridge = 7, 
	fibersplint = 8, 
	removecrown = 9, 
    removepost = 10,
	removebridgeOrSplint = 11
};

struct Procedure
{
    enum FinancingSource { Patient, NHIF, PHIF };
    Procedure(const ProcedureTemplate& t, Date date, std::string diagnosis, int tooth = 99, bool temp = false)
    :
    code{ t.code },
        name{ t.name },
        date{ date },
        financingSource{ static_cast<FinancingSource>(t.nhif) },
        tooth{ tooth },
        temp{ temp },
        diagnosis(diagnosis)

    {
        if (static_cast<int>(t.type) < 7)
            type = static_cast<ProcedureType>(t.type);
        else
            type = static_cast<ProcedureType>(static_cast<int>(t.type) + 1);

    }

    Procedure() {}
    //common parameters:
    ProcedureType type{ ProcedureType::general };
    int code{ -1 };
    Date date;
    std::string name;
    std::string diagnosis;
 //   double price{ 0 };
    int tooth{ -1 };        //-1 for general/several teeth, any in range 0-31 for specific tooth
    bool temp{ false };
    Result result{ NoData{} };
    FinancingSource financingSource{ Patient };
    std::string LPK;
    std::string ksmp;
    std::string notes;
   // Procedure() {};

    void applyProcedure(ToothContainer& teeth) const;
    //applies the procedures, not taking data into account
    void applyPISProcedure(ToothContainer& teeth) const;

    bool isNhif() const {
        return financingSource == FinancingSource::NHIF;
    }


};




