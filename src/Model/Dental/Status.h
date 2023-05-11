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

class Tooth;

struct Dsn : public Status
{
private:
    std::unique_ptr<Tooth> dsn_tooth;
public:
    //call if the tooth can have supernumeral twin
    void init(int idx);

    Dsn() {};

    Dsn(const Dsn& other);

    Dsn& operator= (const Dsn& other);

    const Tooth& tooth() const { return *dsn_tooth; }
    Tooth& tooth() { return *dsn_tooth; }
};
