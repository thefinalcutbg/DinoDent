#pragma once
#include <string>
#include <unordered_map>
#include <map>

enum class ProcedureType
{
    none = 0,
    general = 1,
    obturation = 2,
    endo = 3,
    extraction = 4,
    crown = 5,
    implant = 6,
    bridge = 7,
    fibersplint = 8,
    denture = 9,
    removePost = 10,
    removeCrown = 11,
    depuratio = 12,
    anesthesia = 13 
};


class ProcedureCode
{
    struct Numenclature {
        ProcedureType type;
        std::string name;
        std::string ksmp; //empty if not covered by NHIF
        int oldCode;
        int hisType;
    };

    inline static const std::string dummy;

    inline static std::unordered_map<std::string, ProcedureCode::Numenclature> s_mapping;
    inline static std::map<int, std::string> nhif_procedures;

    std::string m_code;

public:

    ProcedureCode() {}
    ProcedureCode(const std::string& code);
    ProcedureCode(int oldCode);

    static void initialize();
    static const std::map<int, std::string>& procedureByNhifCode() {
        return nhif_procedures;
    };
    static std::vector<ProcedureCode> getNonNhifProcedures();
    

    bool isValid() const {
        return s_mapping.count(m_code);
    }

    int oldCode() const;
    const std::string& ksmp() const;
    const std::string& name() const;
    const std::string& code() const { return m_code; }
    int hisType() const;
    ProcedureType type() const;

    static decltype(s_mapping) getMap() { return s_mapping; }

};