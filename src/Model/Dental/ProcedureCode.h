#pragma once
#include <string>
#include <unordered_map>
#include <map>

enum class ProcedureType
{
    none = 0,
    general = 1,
    any = 2,
    obturation = 3,
    extraction = 4,
    endo = 5,
    crown = 6,
    implant = 7,
    bridge = 8,
    fibersplint = 9,
    removecrown = 10,
    removepost = 11,
    removebridgeOrSplint = 12
};


class ProcedureCode
{
    struct Numenclature {
        ProcedureType type;
        std::string name;
        std::string ksmp;
        int nhifCode;
    };

    inline static const std::string dummy;

    inline static std::unordered_map<std::string, ProcedureCode::Numenclature> s_mapping;
    inline static std::map<int, std::string> nhif_procedures;

    std::string m_code;

public:

    ProcedureCode() {}
    ProcedureCode(const std::string& code);
    ProcedureCode(int nhifCode);

    static void initialize();
    static const std::map<int, std::string>& procedureByNhifCode() {
        return nhif_procedures;
    };
    static std::vector<ProcedureCode> getNonNhifProcedures();
    

    bool isValid() const {
        return s_mapping.count(m_code);
    }

    int nhifCode() const;
    const std::string& ksmp() const;
    const std::string& name() const;
    const std::string& code() const { return m_code; }
    ProcedureType type() const;

};