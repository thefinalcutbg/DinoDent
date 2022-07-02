#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "StatusData.h"
#include "Enums.h"
#include "Model/User/User.h"

class Status
{
protected:
    bool m_exists{ 0 };

public:
    virtual bool exists() const { return m_exists; }
    virtual void set(bool exists) { this->m_exists = exists; }

};

struct DentistMade : public Status
{
    std::string LPK;
};


struct Pathology : public Status {PathologyData data; };
struct Obturation : public DentistMade { ObturationData data; };

struct Construction : virtual public DentistMade { BridgePos position{ BridgePos::Middle }; };
struct Crown : virtual public DentistMade { CrownData data; };
struct Bridge : public Crown, public Construction {};
struct FiberSplint : public ObturationData, public Construction {};

struct Implant : public DentistMade { ImplantData data; };
struct Mobility : public Status { Degree degree{ Degree::First };};


