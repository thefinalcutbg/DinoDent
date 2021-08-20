#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "StatusData.h"
#include "Enums.h"
#include "Model/User/CurrentUser.h"

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

    DentistData getDentistData() const
    {
        auto& currentUser = CurrentUser::instance();
        
        if (LPK.empty())
            return DentistData{ currentUser.getName(), true, false };

        if (LPK == currentUser.LPK)
            return DentistData{ currentUser.getName(), true, true };

        return DentistData{ LPK, false, true };
    }

    void setUser(bool isPresent)
    {
        if (!isPresent) { 
            LPK.clear();
            return;
        }

        if (LPK.empty()) 
            LPK = CurrentUser::instance().LPK;
    }

};

struct Pathology : public Status {PathologyData data; };
struct Obturation : public DentistMade { ObturationData data; };
struct Crown : public DentistMade { CrownData data; };
struct Bridge : DentistMade { CrownData data; BridgePos position{ BridgePos::Middle }; };
struct Implant : public DentistMade { ImplantData data; };
struct Mobility : public Status { Degree degree{ Degree::First };};


