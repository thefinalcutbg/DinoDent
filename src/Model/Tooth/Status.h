#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "Enums.h"
#include "Vita.h"

class Status
{
protected:
    bool m_exists;

    const std::string* default_attribute;

public:

    Status() : m_exists(0), default_attribute(nullptr) {}
    virtual bool exists() const { return m_exists; }
    virtual void set(bool exists) { this->m_exists = exists; }

    void setDefaultAttribute(const std::string& defaultValue)
    {
        default_attribute = &defaultValue;
    }

};


class Pathology : public Status
{

public:
    std::string diagnosis;
    std::string date_diagnosed;

    virtual void set(bool exists)
    {
        m_exists = exists;
        if (exists && date_diagnosed.empty()) {
            date_diagnosed = Date::toString(Date::currentDate());
        }
    }

    std::string getDiagnosis() const
    {
        if (!diagnosis.empty())
        {
            return diagnosis;
        }

        if (default_attribute != NULL)
        {
            return *default_attribute;
        }

        return diagnosis;
    }
};





struct DentistMade : public Status
{
    std::string LPK;
};

struct Material : public DentistMade
{
    std::string material;

    std::string getMaterial()
    {
        if (!material.empty())
        {
            return material;
        }

        if (default_attribute != NULL)
        {
            return *default_attribute;
        }

        return material;
    }
};


struct Obturation : public Material
{
    int color{ 0 };
};

struct Crown : public Material
{
    int prep_type{ 0 };
    Vita color;
};


struct Bridge : public Crown
{
    BridgePos position;
};

struct Implant : public DentistMade
{
    std::string system;

    double width{ 0 }, length{ 0 };
    int time{ 0 }, type{ 0 }, tissue_aug{ 0 }, bone_aug{ 0 };
    bool membrane{ false }, sinusLift{ false };
};

struct Mobility : public Status
{
    Mobility() : degree(Degree::First) {}
    Degree degree;
};
