#pragma once
#include <string>
#include <array>
#include <variant>
#include "Model/Date.h"
#include "ProcedureTemplate.h"
#include "../Tooth/StatusData.h"
class ToothContainer;

struct NoData {};

struct ProcedureBridgeData {

    int tooth_begin{ 0 };
    int tooth_end{ 0 };

    CrownData crown;
};

struct ProcedureObtData
{
    std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
    bool post{ false };

    ObturationData data;
};

struct ProcedureFiberData
{
    int tooth_begin{ 0 };
    int tooth_end{ 0 };
    
    ObturationData obtur;

};


struct Anesthesia {
    int minutes{ 0 };
};

typedef std::variant<NoData, ProcedureObtData, CrownData, ProcedureBridgeData, ImplantData, ProcedureFiberData, Anesthesia> Result;

struct Procedure
{


    Procedure(const ProcedureTemplate& t, Date date, std::string name, std::string diagnosis, double price, Result result = NoData{}, int tooth = -1, bool temp = false)
        :
        type{ t.type },
        code{ t.code },
        date{ date },
        name{ name },
        diagnosis{ diagnosis },
        price{ price },
        tooth{ tooth },
        temp{ temp },
        nzok{ t.nzok },
        result{ result }
    {}

    Procedure() {};

    //common parameters:
    ProcedureType type{ ProcedureType::general };
    int code{ -1 };
    Date date;
    std::string name;
    std::string diagnosis;
    double price{ -1 };
    int tooth{ -1 };        //-1 for general/several teeth, any in range 0-31 for specific tooth
    bool temp{ false };
    Result result{ NoData{} };
    bool nzok{ false };
    std::string LPK;

    void applyProcedure(ToothContainer& teeth);


};




