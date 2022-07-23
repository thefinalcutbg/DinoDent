#pragma once
#include "../Date.h"
#include <vector>
#include <array>

struct DentistData
{
    std::string dentistName;
    bool isEnabled;
    bool isChecked;
};

class PathologyData
{
    const std::vector<std::string>* diagnosis_list{ nullptr };

public:

    int diagnosis_index{ 0 };
    Date date_diagnosed{ Date::currentDate() };

    void setDiagnosisList(const std::vector<std::string>& diagnosisList){
        diagnosis_list = &diagnosisList;
    }

    const std::vector<std::string>* diagnosisList() const { return diagnosis_list; }

    std::string getDiagnosisName() const 
    {
        if (diagnosis_list == nullptr || !(diagnosis_index < diagnosis_list->size()))
            return std::string{};

        return diagnosis_list->at(diagnosis_index);
    }
};

struct CrownData {

    std::string material;
    int prep_type{ 0 };
    int color{0};

    std::string infoStr();

    static const std::array<std::string_view, 5>& prepTypes();

};


struct ObturationData
{
    int color{ 0 };
    std::string material;

    std::string infoStr() const;
    static const std::array <std::string_view, 17>& colorStrings();
};

struct ImplantData 
{
    std::string system;
    double width{ 0 }, length{ 0 };
    int time{ 0 }, type{ 0 }, tissue_aug{ 0 }, bone_aug{ 0 };
    bool membrane{ false }, sinusLift{ false };

    static const std::array <std::string_view, 4>& typeStr();
    static const std::array <std::string_view, 4>& timeStr();
    static const std::array <std::string_view, 4>& tissueAugStr();
    static const std::array <std::string_view, 5>& boneAugStr();

    std::string infoStr() const;
};