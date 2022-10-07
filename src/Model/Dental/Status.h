#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "StatusData.h"
#include "Enums.h"
#include "Model/User.h"



class Status
{
protected:
    bool m_exists{ 0 };
    virtual std::string getInfo() const { return {}; }
public:
    virtual bool exists() const { return m_exists; }
    virtual void set(bool exists) { m_exists = exists; }
    std::string info() const { return m_exists ? getInfo() : ""; }

    operator bool() const { return m_exists; };
};

struct DentistMade : public Status
{
    std::string LPK;
};


struct Pathology : public Status 
{
    PathologyData data;
private:
    std::string getInfo() const override {
        return "<b><font color=\"red\">" + data.getDiagnosisName() + "</font></b>"
            + u8"<br>(диагностициран на " + data.date_diagnosed.toBgStandard(true) + ")";
    }
};
struct Obturation : public DentistMade
{ 
    ObturationData data; 
private:
    std::string getInfo() const override;
  
};
//DIAMOND INHERITANCE PROBLEM...
struct Construction : virtual public DentistMade { BridgePos position{ BridgePos::Middle }; 
};
struct Crown : virtual public DentistMade { 
    CrownData data; 
};
//...EMERGES HERE:
struct Bridge : public Crown, public Construction {};
struct FiberSplint : public Construction { ObturationData data; };

struct Implant : public DentistMade { ImplantData data; };
struct Mobility : public Status { Degree degree{ Degree::First };};


