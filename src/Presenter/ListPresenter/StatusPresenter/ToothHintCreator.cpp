#include "ToothHintCreator.h"
#include "Model/User/CurrentUser.h"
#include "Model/Tooth/Tooth.h"

bool R_U_Mine(const DentistMade& procedure)
{
   
    return procedure.LPK == CurrentUser::instance().LPK;
}

ToothPaintHint ToothHintCreator::getToothHint(const Tooth& tooth)
{
    ToothPaintHint hint;

    hint.idx = tooth.index;
    hint.temp = tooth.temporary.exists();
    hint.num = ToothUtils::getToothNumber(tooth.index, hint.temp);

    //the tooth hint:
    hint.tooth = ToothTextureHint::normal;

    if (tooth.extraction.exists())
    {
        if (tooth.bridge.exists())
            hint.tooth = ToothTextureHint::none;

        else if (R_U_Mine(tooth.extraction))
            hint.tooth = ToothTextureHint::extr_m;

        else
            hint.tooth = ToothTextureHint::extr;
    }
    else if (tooth.implant.exists())
    {
        if (R_U_Mine(tooth.implant))
            hint.tooth = ToothTextureHint::impl_m;
        else
            hint.tooth = ToothTextureHint::impl;
    }
    else if (tooth.root.exists())
    {
        hint.tooth = ToothTextureHint::root;
    }
    
    //surface hint

    for (int i = 0; i < hint.surfaces.size(); i++)
    {
        hint.surfaces[i] = SurfaceHint{ SurfaceColor::none, false };

        if (tooth.obturation.exists(i) && tooth.caries.exists(i))
        {
            hint.surfaces[i].outline = true;
        }

        if (tooth.obturation.exists(i))
        {
            if (R_U_Mine(tooth.obturation[i]))
                hint.surfaces[i].color = SurfaceColor::green;
            else
                hint.surfaces[i].color = SurfaceColor::blue;
        }
        else if (tooth.caries.exists(i))
            hint.surfaces[i].color = SurfaceColor::red;
    }

    //endo hint

    hint.endo = EndoHint::none;

    if (tooth.endo.exists())
    {
        if (R_U_Mine(tooth.endo)) hint.endo = EndoHint::green;
        else hint.endo = EndoHint::blue;
    }
    else if (tooth.pulpitis.exists())
        hint.endo = EndoHint::red;
    else if (tooth.lesion.exists() && !tooth.implant.exists())
        hint.endo = EndoHint::darkred;

    //prosthodontic hint

    hint.prostho = ProsthoHint::none;

    if (tooth.crown.exists())
    {
        R_U_Mine(tooth.crown) ? 
            hint.prostho = ProsthoHint::crown_green
            :
            hint.prostho = ProsthoHint::crown;
    }
    else if (tooth.bridge.exists())
    {
        R_U_Mine(tooth.bridge) ?
            hint.prostho = ProsthoHint::bridge_green
            :
            hint.prostho = ProsthoHint::bridge;
    }
    
    hint.post = PostHint::none;

    if (tooth.post.exists())
    {
        R_U_Mine(tooth.post) ?
            hint.post = PostHint::green
            :
            hint.post = PostHint::blue;
    }

    hint.dns = tooth.hyperdontic.exists();
    hint.frac = tooth.fracture.exists();
    hint.perio = tooth.periodontitis.exists();
    hint.lesion = tooth.lesion.exists();

    hint.mobility = 0;

    if (tooth.mobility.exists())
        hint.mobility = static_cast<int>(tooth.mobility.degree) + 1;

    return hint;
}

std::array<ToothPaintHint, 32> ToothHintCreator::getTeethHint(const std::array<Tooth, 32>& teeth)
{
    std::array<ToothPaintHint, 32> paintHints;

    for (int i = 0; i < 32; i++)
    {
        paintHints[i] = getToothHint(teeth[i]);
    }

    return paintHints;
}



BridgesPaintHint ToothHintCreator::statusToUIBridge(std::array<Tooth, 32>& teeth)
{

    BridgesPaintHint paintHint;

    for (int i = 0; i < teeth.size(); i++) {

        Tooth& tooth = teeth.at(i);
        BridgeAppearance appearance;

        if (!tooth.bridge.exists())
        {
            appearance = BridgeAppearance::None;
        }
        else if (tooth.bridge.position == BridgePos::Middle) {
            appearance = BridgeAppearance::Middle;
        }
        else {
            appearance = BridgeAppearance::Terminal;
        }

        paintHint.paintHints[i] = std::make_tuple(appearance, R_U_Mine(tooth.bridge));
    }

    return paintHint;
}