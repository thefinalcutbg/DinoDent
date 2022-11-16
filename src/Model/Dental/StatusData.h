#pragma once
#include "../Date.h"
#include <vector>
#include <array>
#include "VitaColor.h"
#include "DentalNum.h"

struct DentistData
{
    std::string dentistName;
    bool isEnabled;
    bool isChecked;
};

struct ObturationData
{
    VitaColor color;
    RestorationMaterial material;

    std::string infoStr() const;
};


struct PostData {

    PostType type;
    std::string infoStr() const { return type.getName(); }
};

struct CrownData {

    CrownMaterial material;
    CrownPreparation prep;
    VitaColor color;

    std::string infoStr() const;
};


struct ImplantData 
{
    ImplantTime time;
    ImplantType type;

    double width{ 0 }, length{ 0 };

    TissueAugmentation tissue_aug;
    BoneAugmentation bone_aug;

    bool membrane{ false }, sinusLift{ false };

    std::string infoStr() const;
};