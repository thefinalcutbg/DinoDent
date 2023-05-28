#pragma once
#include <string>
#include <unordered_map>
#include <map>

enum class ProcedureType
{
    general = 0,
    full_exam = 1,
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
    static std::vector<ProcedureCode> getByType(ProcedureType t);

    bool isValid() const {
        return s_mapping.count(m_code);
    }

    int oldCode() const;
    const std::string& ksmp() const;
    const std::string& name() const;
    const std::string& code() const { return m_code; }
    int hisType() const;
    ProcedureType type() const;

    bool isToothSpecific()const {

        auto type = this->type();

        return
            type != ProcedureType::general &&
            type != ProcedureType::full_exam &&
            type != ProcedureType::bridge &&
            type != ProcedureType::fibersplint &&
            type != ProcedureType::denture &&
            type != ProcedureType::depuratio
            // && type != ProcedureType::removebridgeOrSplint
            ;
    }
    bool isRestoration() const {
        return type() == ProcedureType::obturation;
    }
    bool isRangeSpecific() const {

        return
            type() == ProcedureType::bridge ||
            type() == ProcedureType::fibersplint ||
            type() == ProcedureType::denture;
    }
    bool isAnesthesia() const {
        return type() == ProcedureType::anesthesia;
    }

    static decltype(s_mapping) getMap() { return s_mapping; }

};