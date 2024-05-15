#include "PaintHint.h"

#include "Model/User.h"
#include "Model/Dental/Tooth.h"
#include "Model/Dental/ToothUtils.h"

using namespace Dental;

//I'm a puppet on a string
bool R_U_Mine(const Tooth& tooth, int code)
{
    return User::isCurrentUser(tooth.getLPK(code));
}

BridgeTerminal getBridgePosition(int toothIdx, BridgePos position)
{
    if (position == BridgePos::Middle)
        return BridgeTerminal::Center;

    Quadrant q = ToothUtils::getQuadrant(toothIdx);

    if (q == Quadrant::First || q == Quadrant::Third)
        return position == BridgePos::Begin ? BridgeTerminal::Distal : BridgeTerminal::Medial;
    else
        return position == BridgePos::Begin ? BridgeTerminal::Medial : BridgeTerminal::Distal;

}

ToothPaintHint::ToothPaintHint(const Tooth& tooth, const std::string& notes)
{
    idx = tooth.index();
    temp = tooth[Temporary];
    num = ToothUtils::getNhifNumber(tooth.toothIndex());

    //the tooth hint:
    if (tooth.noData()) {
        this->tooth = ToothTextureHint::unknown;
        goto getToolTip;
        return;
    }

    calculus = tooth[Calculus];
    frac = tooth[Fracture];
    perio = tooth[Periodontitis];
    lesion = tooth[ApicalLesion];
    impacted = tooth[Impacted];
    mobility = 0;

    if (tooth[Mobility])
        mobility = static_cast<int>(tooth.m_degree) + 1;

    this->tooth = ToothTextureHint::normal;

    if (tooth[Missing])
    {
        if (tooth[Bridge] || tooth[Splint])
            this->tooth = ToothTextureHint::none;

        else if (R_U_Mine(tooth, Missing))
            this->tooth = ToothTextureHint::extr_m;

        else
            this->tooth = ToothTextureHint::extr;
    }
    else if (tooth[Implant])
    {
        if (R_U_Mine(tooth, Implant))
            this->tooth = ToothTextureHint::impl_m;
        else
            this->tooth = ToothTextureHint::impl;
    }
    else if (tooth[Root])
    {
        this->tooth = ToothTextureHint::root;
    }


    //surface hint

    for (int i = 0; i < surfaces.size(); i++)
    {
        surfaces[i] = SurfaceHint{ SurfaceColor::none, false };

        surfaces[i].outline = tooth.hasSecondaryCaries(i);

        if (tooth.hasRestoration(i))
        {
            if (R_U_Mine(tooth, i))
                surfaces[i].color = SurfaceColor::green;
            else
                surfaces[i].color = SurfaceColor::blue;
        }
        else if (tooth.hasCaries(i))
            surfaces[i].color = SurfaceColor::red;
    }

    //endo hint

    endo = EndoHint::none;

    if (tooth[RootCanal])
    {
        if (R_U_Mine(tooth, RootCanal)) endo = EndoHint::green;
        else endo = EndoHint::blue;
    }
    else if (tooth[Pulpitis])
        endo = EndoHint::red;
    else if (tooth[ApicalLesion] && !tooth[Implant])
        endo = EndoHint::darkred;

    //prosthodontic hint

    prostho = ProsthoHint::none;

    if (tooth[Crown])
    {
        R_U_Mine(tooth, Crown) ?
            prostho = ProsthoHint::crown_green
            :
            prostho = ProsthoHint::crown;
    }
    else if (tooth[Bridge])
    {
        R_U_Mine(tooth, Bridge) ?
            prostho = ProsthoHint::bridge_green
            :
            prostho = ProsthoHint::bridge;

        bridgePos = getBridgePosition(tooth.index(), tooth.position);
    }
    else if (tooth[Splint])
    {
        R_U_Mine(tooth, Splint) ?
            prostho = ProsthoHint::splint_green
            :
            prostho = ProsthoHint::splint;

        bridgePos = getBridgePosition(tooth.index(), tooth.position);

    }

    else if (tooth[Denture])
    {
        prostho = R_U_Mine(tooth, Denture) ?
            ProsthoHint::denture_green
            :
            ProsthoHint::denture;

        if (!impacted &&
            this->tooth != ToothTextureHint::root &&
            this->tooth != ToothTextureHint::impl &&
            this->tooth != ToothTextureHint::impl_m
          ) this->tooth = ToothTextureHint::none;

    }

    post = PostHint::none;

    if (tooth[Post])
    {
        R_U_Mine(tooth, Post) ?
            post = PostHint::green
            :
            post = PostHint::blue;
    }

    if (tooth[HasSupernumeral])
    {
        dsn = std::make_unique<ToothPaintHint>(tooth.getSupernumeral());
        dsn->isHyperdontic = true;
    }
getToolTip:
    toolTip = "<b>" + tooth.toothName() + "</b><br>";
    toolTip += tooth.getToothInfo();
    if (notes.size()) {
        toolTip += "<br><br><b>Бележки:</b> " + notes;
    }
}
