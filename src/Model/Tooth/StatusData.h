#pragma once
#include "../Date.h"
#include <vector>

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
};


struct ObturationData
{
    int color{ 0 };
    std::string material;
};

struct ImplantData 
{
    std::string system;
    double width{ 0 }, length{ 0 };
    int time{ 0 }, type{ 0 }, tissue_aug{ 0 }, bone_aug{ 0 };
    bool membrane{ false }, sinusLift{ false };
};