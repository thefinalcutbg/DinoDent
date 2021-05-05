#include "ToothHintCreator.h"


bool R_U_Mine(const DentistMade& procedure)
{
   std::string usr_LPK = "220008771";
   return procedure.LPK == usr_LPK;
}

PaintHint ToothHintCreator::getToothHint(const Tooth& tooth)
{
    PaintHint hint;

    hint.idx = tooth.index;
    hint.temp = tooth.temporary.exists();
    hint.num = ToothUtils::getToothNumber(tooth.index, hint.temp);

    //the tooth hint:
    hint.tooth = ToothHint::normal;

    if (tooth.extraction.exists())
    {
        if (tooth.bridge.exists())
            hint.tooth = ToothHint::none;

        else if (R_U_Mine(tooth.extraction))
            hint.tooth = ToothHint::extr_m;

        else
            hint.tooth = ToothHint::extr;
    }
    else if (tooth.implant.exists())
    {
        if (R_U_Mine(tooth.implant))
            hint.tooth = ToothHint::impl_m;
        else
            hint.tooth = ToothHint::impl;
    }
    else if (tooth.root.exists())
    {
        hint.tooth = ToothHint::root;
    }
    
    //surface hint

    for (int i = 0; i < hint.surfaces.size(); i++)
    {
        hint.surfaces[i] = SurfaceHint::none;

        if (tooth.obturation.exists(i) && tooth.caries.exists(i))
        {
            hint.surfaces[i] = SurfaceHint::normal;
        }
        else if (tooth.obturation.exists(i))
        {
            if (R_U_Mine(tooth.obturation[i]))
                hint.surfaces[i] = SurfaceHint::green;
            else
                hint.surfaces[i] = SurfaceHint::blue;
        }
        else if (tooth.caries.exists(i))
            hint.surfaces[i] = SurfaceHint::red;
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
        if (R_U_Mine(tooth.crown)) hint.prostho = ProsthoHint::crown_green;
        else hint.prostho = ProsthoHint::crown;
    }
    else if (tooth.bridge.exists())
    {
        if (R_U_Mine(tooth.bridge))
        {
            switch (tooth.bridge.position)
            {
                case BridgePos::Begin: hint.prostho = ProsthoHint::br_b_green; break;
                case BridgePos::Middle: hint.prostho = ProsthoHint::br_m_green; break;
                case BridgePos::End: hint.prostho = ProsthoHint::br_e_green; break;
            }
        }
        else
        {
            switch (tooth.bridge.position)
            {
                case BridgePos::Begin: hint.prostho = ProsthoHint::br_b; break;
                case BridgePos::Middle: hint.prostho = ProsthoHint::br_m; break;
                case BridgePos::End: hint.prostho = ProsthoHint::br_e; break;
            }
        }
    }
    
    hint.post = PostHint::none;

    if (tooth.post.exists())
    {
        if (R_U_Mine(tooth.post)) hint.post = PostHint::green;
        else hint.post = PostHint::blue;
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
