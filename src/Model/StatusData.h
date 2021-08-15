#pragma once
#include "Date.h"

struct PathologyData
{
    Date date_diagnosed;
    std::string diagnosis;

};

struct DentistData
{
    std::string dentist_uin;
};

struct CrownData {

    std::string material;
    int prep_type{ 0 };
    int color{0};
};


struct ObturationData {
    int color{ 0 };
    std::string material;
};

struct ImplantData {
    std::string system;
    double width{ 0 }, length{ 0 };
    int time, type, tissue_aug, bone_aug;
    bool membrane, sinusLift;
};