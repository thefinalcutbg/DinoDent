#pragma once
#include <string>
#include <array>
#include <memory>
#include "Model/Date.h"
#include "StatusData.h"
#include "Enums.h"
#include "Model/User.h"
#include "Model/Dental/DentalNum.h"
#include "Model/FreeFunctions.h"


class Status
{

protected:
    bool m_exists{ 0 };
    virtual std::string getInfo() const { return {}; }
public:
    virtual bool exists() const { return m_exists; }
    virtual void set(bool exists) { 
        m_exists = exists; 
    }
    std::string info() const { return m_exists ? getInfo() : ""; }

    operator bool() const { return m_exists; };
};

struct DentistMade : public Status
{
    std::string LPK;
};

struct Construction : virtual public DentistMade {
    BridgePos position{ BridgePos::Middle };
};

struct Mobility : public Status 
{ 
    Degree degree{ Degree::First }; 

    std::array<bool, 3> getBoolStatus() const {
        std::array<bool, 3> result{ false, false, false };
        if(m_exists) result[static_cast<int>(degree)] = true;
        return result;
    }
};
