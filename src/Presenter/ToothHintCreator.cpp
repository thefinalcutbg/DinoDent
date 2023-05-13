#include "ToothHintCreator.h"
#include "Model/User.h"
#include "Model/Dental/ToothContainer.h"

//I'm a puppet on a string
bool R_U_Mine(const DentistMade& procedure)
{
   
    return User::isCurrentUser(procedure.LPK);
}

BridgeTerminal getPosition(int toothIdx, BridgePos position)
{
    if (position == BridgePos::Middle) 
        return BridgeTerminal::Center;

    Quadrant q = ToothUtils::getQuadrant(toothIdx);

    if (q == Quadrant::First || q == Quadrant::Third)
        return position == BridgePos::Begin ? BridgeTerminal::Distal : BridgeTerminal::Medial;
    else
        return position == BridgePos::Begin ? BridgeTerminal::Medial : BridgeTerminal::Distal;

}

ToothPaintHint ToothHintCreator::getToothHint(const Tooth& tooth, const std::string& notes)
{
    ToothPaintHint hint;

    hint.idx = tooth.index;
    hint.temp = tooth.temporary.exists();
    hint.num = ToothUtils::getToothNumber(tooth.index, hint.temp);

    //the tooth hint:
    if (tooth.noData()) {
        hint.tooth = ToothTextureHint::unknown;
        return hint;
    }

    hint.calculus = tooth.calculus.exists();
    hint.frac = tooth.fracture.exists();
    hint.perio = tooth.periodontitis.exists();
    hint.lesion = tooth.lesion.exists();
    hint.impacted = tooth.impacted.exists();
    hint.mobility = 0;

    if (tooth.mobility.exists())
        hint.mobility = static_cast<int>(tooth.mobility.degree) + 1;

    hint.tooth = ToothTextureHint::normal;

    if (tooth.extraction.exists())
    {
        if (tooth.bridge.exists() || tooth.splint.exists())
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

        hint.bridgePos = getPosition(tooth.index, tooth.bridge.position);
    }
    else if (tooth.splint.exists())
    {
        R_U_Mine(tooth.splint) ?
            hint.prostho = ProsthoHint::splint_green
            :
            hint.prostho = ProsthoHint::splint;

        hint.bridgePos = getPosition(tooth.index, tooth.splint.position);

    }

    else if (tooth.denture.exists())
    {
        hint.prostho = ProsthoHint::denture;
        
        if(  !hint.impacted &&
             hint.tooth != ToothTextureHint::root && 
             hint.tooth != ToothTextureHint::impl &&
             hint.tooth != ToothTextureHint::impl_m
        ) hint.tooth = ToothTextureHint::none;
        
    }
    
    hint.post = PostHint::none;

    if (tooth.post.exists())
    {
        R_U_Mine(tooth.post) ?
            hint.post = PostHint::green
            :
            hint.post = PostHint::blue;
    }

    if (tooth.dsn)
    {
        hint.dsn = std::make_unique<ToothPaintHint>(tooth.dsn.tooth());
        hint.dsn->isHyperdontic = true;
    }

    hint.toolTip = "<b>" +  tooth.toothName() + "</b><br>";

    if (notes.size()) {
        hint.toolTip += "<br><b>Бележки:</b><br>" + notes;
    }

    return hint;
}

std::array<ToothPaintHint, 32> ToothHintCreator::getTeethHint(const ToothContainer& teeth)
{
    std::array<ToothPaintHint, 32> paintHints;

    for (int i = 0; i < 32; i++)
    {
        paintHints[i] = getToothHint(teeth[i]);
    }

    return paintHints;
}