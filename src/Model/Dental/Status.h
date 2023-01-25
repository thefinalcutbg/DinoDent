#pragma once
#include <string>
#include <array>
#include "Model/Date.h"
#include "StatusData.h"
#include "Enums.h"
#include "Model/User.h"
#include "Model/Dental/DentalNum.h"

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
    Date date_diagnosed{ Date::currentDate() };
/*
    virtual std::string getDiagnosisString() const = 0;
    virtual bool setDiagnosisIdx(int idx) = 0;
    virtual int getDiagnosisIdx() const = 0;
    virtual std::vector<std::string> getDiagnosisList() const = 0;

private:
    std::string getInfo() const override {
        return std::string("<b><font color=\"red\">") + getDiagnosisString() + "</font></b>"
            + "<br>(диагностициран на " + date_diagnosed.toBgStandard(true) + ")";
    }
        */
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

/*
struct Caries : public Pathology
{
    
    CariesDiagnosis diag;
public:
    std::string getDiagnosisString() const override { return diag.getName(); }
    bool setDiagnosisIdx(int idx) override { return diag.setIndex(idx); }
    virtual int getDiagnosisIdx() const override { return diag.getIndex(); }
    std::vector<std::string> getDiagnosisList() const override { return diag.getNamesAsStrings(); }
};

struct Pulpitis : public Pathology
{
    PulpitisDiagnosis diag;
public:
    std::string getDiagnosisString() const override { return diag.getName(); }
    bool setDiagnosisIdx(int idx) override { return diag.setIndex(idx); }
    virtual int getDiagnosisIdx() const override { return diag.getIndex(); }
    std::vector<std::string> getDiagnosisList() const override { return diag.getNamesAsStrings(); }

};

struct Fracture : public Pathology
{
    FractureDiagnosis diag;
public:
    std::string getDiagnosisString() const override { return diag.getName(); }
    bool setDiagnosisIdx(int idx) override { return diag.setIndex(idx); }
    virtual int getDiagnosisIdx() const override { return diag.getIndex(); }
    std::vector<std::string> getDiagnosisList() const override { return diag.getNamesAsStrings(); }

};

struct Root : public Pathology
{
    RootDiagnosis diag;
public:
    std::string getDiagnosisString() const override { return diag.getName(); }
    bool setDiagnosisIdx(int idx) override { return diag.setIndex(idx); }
    virtual int getDiagnosisIdx() const override { return diag.getIndex(); }
    std::vector<std::string> getDiagnosisList() const override { return diag.getNamesAsStrings(); }

};

struct ApicalLesion : public Pathology
{
    ApicalLesionDiagnosis diag;
public:
    std::string getDiagnosisString() const override { return diag.getName(); }
    bool setDiagnosisIdx(int idx) override { return diag.setIndex(idx); }
    virtual int getDiagnosisIdx() const override { return diag.getIndex(); }
    std::vector<std::string> getDiagnosisList() const override { return diag.getNamesAsStrings(); }

};

struct Obturation : public DentistMade
{ 
    ObturationData data; 
private:
    std::string getInfo() const override;
  
};



struct Crown : virtual public DentistMade { 
    CrownData data; 
};


//DIAMOND INHERITANCE PROBLEM...

//...EMERGES HERE:
struct Bridge : public Construction {};
struct FiberSplint : public Construction {};

struct Implant : public DentistMade { ImplantData data; };
struct Post : public DentistMade { PostData data; };


*/
