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
    RemoveRestoration,
    ToothNonSpecific,
    DepuratioTooth,
    Extraction,
    Implant,
    Endodontic,
    Post,
    RemovePost,
    PostCore,
    PostCrown,
    Crown,
    CrownOrBridgeOrVeneer,
    Bridge,
    RemoveCrownOrBridge,
    Denture,
    Splint,
    DepuratioQuadrant,
    MultipleExtraction,
    MaxCount

};


enum class ProcedureScope {
    AllOrNone,
    SingleTooth,
    Range,
    Ambi //Single tooth OR range of teeth
};


class ProcedureCode
{
    struct Numenclature {
        ProcedureType type;
        std::string name;
        int nhifCode{ false };
        int hisType{ 0 };
        bool isLegacy{ false };
        std::string icd;
    };

    inline static const std::string dummy;

    inline static std::unordered_map<std::string, ProcedureCode::Numenclature> s_mapping;
    inline static std::map<int, std::string> legacy_achi; //the old his procedures dont have achi
    static std::vector<ProcedureCode> s_sorted_codes;

    std::string m_code;

public:

    ProcedureCode() {}
    ProcedureCode(const std::string& code);
    ProcedureCode(int nhifCode);

    static void initialize();
    static const std::map<int, std::string>& procedureByNhifCodeLegacy() {
        return legacy_achi;
    };

    static const std::vector<ProcedureCode>& getProcedures() { return s_sorted_codes; };

    static std::vector<ProcedureCode> getByType(ProcedureType t);

    bool isValid() const;

    //returns NHIF code if there is a NHIF mapping otherwise returns 0
    int nhifCode() const;

    const std::string& name() const;

    const std::string& code() const { return m_code; }

    const std::string& ACHICode() const;

    const std::string& defaultICD10() const;

    int achiBlock() const;

    const std::string& achiBlockName() const;

    ProcedureType type() const;

    ProcedureScope getScope() const;

    bool isLegacy() const;

    static decltype(s_mapping) getMap() { return s_mapping; }
    static ProcedureScope getScope(ProcedureType t);
};
