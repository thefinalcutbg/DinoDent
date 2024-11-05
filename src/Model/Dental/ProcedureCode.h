#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

enum class ProcedureType
{
    General = 0,
    FullExam,
    Depuratio,
    DenturePair,
    Anesthesia,
    Restoration,
    ToothSpecific,
    DepuratioTooth,
    Extraction,
    Implant,
    Endodontic,
    Post,
    RemovePost,
    Crown,
    PostCore,
    PostCrown,
    CrownOrBridge,
    Bridge,
    RemoveCrownOrBridge,
    Denture,
    Splint,
    DepuratioQuadrant,
    MultipleExtraction

};


class ProcedureCode
{
    struct Numenclature {
        ProcedureType type;
        std::string name;
        int oldCode;
        int hisType;
        bool isLegacy;
    };

    inline static const std::string dummy;

    inline static std::unordered_map<std::string, ProcedureCode::Numenclature> s_mapping;
    inline static std::map<int, std::string> legacy_achi; //the old his procedures dont have achi
    inline static std::vector<ProcedureCode> s_sorted_codes;

    std::string m_code;

public:

    ProcedureCode() {}
    ProcedureCode(const std::string& code);
    ProcedureCode(int oldCode);

    static void initialize();
    static const std::map<int, std::string>& procedureByNhifCodeLegacy() {
        return legacy_achi;
    };

    static const std::vector<ProcedureCode>& getProcedures() { return s_sorted_codes; };

    static std::vector<ProcedureCode> getByType(ProcedureType t);

    bool isValid() const {
        return s_mapping.count(m_code);
    }

    //returns NHIF code if there is a NHIF mapping otherwise returns 0
    int oldCode() const;

    const std::string& name() const;

    const std::string& code() const { return m_code; }

    const std::string& ACHICode() const;

    int achiBlock() const;

    const std::string& achiBlockName() const;

    ProcedureType type() const;

    //Doesnt need teeth to apply its behaviour
    bool isGeneral() const {

        return type() < ProcedureType::Restoration;
    }

    //Affects single tooth
    bool isToothSpecific()const {

        auto type = this->type();

        return type >= ProcedureType::Restoration && type <= ProcedureType::PostCrown;
    }

    //Affects range of teeth (from - to)
    bool isRangeSpecific() const {

        return type() >= ProcedureType::CrownOrBridge;
    }

    bool isLegacy() const {
        
        if (m_code.empty()) return true;

        return s_mapping[m_code].isLegacy;
    }

    bool isRestoration() const {
        return type() == ProcedureType::Restoration;
    }

    bool isAnesthesia() const {
        return type() == ProcedureType::Anesthesia;
    }

    bool requiresRangeValidation() const {

        auto codeType = type();

        return codeType >= ProcedureType::Bridge && codeType < ProcedureType::DepuratioQuadrant;
    }

    static decltype(s_mapping) getMap() { return s_mapping; }

};
