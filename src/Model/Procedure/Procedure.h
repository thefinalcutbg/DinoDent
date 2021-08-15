#pragma once
#include <string>
#include <array>
#include <variant>
#include "Model/Date.h"
#include "ProcedureTemplate.h"
#include "../StatusData.h"
class ToothContainer;

struct NoData {};

struct BridgeData {

    CrownData crown;

    int tooth_begin{ 0 };
    int tooth_end{ 0 };
};

struct PObturationData : ObturationData
{
    std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
    bool post{ false };
};

struct Anesthesia {
    int minutes{ 0 };
};

typedef std::variant<NoData, PObturationData, CrownData, BridgeData, ImplantData, Anesthesia> Result;

struct Procedure
{


    Procedure(const ProcedureTemplate& t, Date date, std::string name, std::string diagnosis, double price, Result result, int tooth = -1)
        :
        type{ t.type },
        code{ t.code },
        date{ date },
        name{ name },
        diagnosis{ diagnosis },
        price{ price },
        tooth{ tooth },
        nzok{ t.nzok },
        result{result}
    {}

    Procedure() : type(ProcedureType::general), code{ -1 }, price{ 0 }, tooth{ -1 }, nzok{ false } {}



    //common parameters:
    ProcedureType type;
    int code;
    Date date;
    std::string name;
    std::string diagnosis;
    double price;
    int tooth;
    Result result;

    std::string LPK;

    void applyProcedure(ToothContainer& teeth);

    //NZOK specific:
    bool nzok;
};




