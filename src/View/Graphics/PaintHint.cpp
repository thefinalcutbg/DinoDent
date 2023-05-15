#include "PaintHint.h"

#include "Model/User.h"
#include "Model/Dental/Tooth.h"

//I'm a puppet on a string
bool R_U_Minee(const DentistMade& procedure)
{

    return User::isCurrentUser(procedure.LPK);
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
    idx = tooth.index;
    temp = tooth.temporary.exists();
    num = ToothUtils::getNhifNumber(tooth.index, temp, false);

    //the tooth hint:
    if (tooth.noData()) {
        this->tooth = ToothTextureHint::unknown;
        return;
    }

    calculus = tooth.calculus.exists();
    frac = tooth.fracture.exists();
    perio = tooth.periodontitis.exists();
    lesion = tooth.lesion.exists();
    impacted = tooth.impacted.exists();
    mobility = 0;

    if (tooth.mobility.exists())
        mobility = static_cast<int>(tooth.mobility.degree) + 1;

    this->tooth = ToothTextureHint::normal;

    if (tooth.extraction.exists())
    {
        if (tooth.bridge.exists() || tooth.splint.exists())
            this->tooth = ToothTextureHint::none;

        else if (R_U_Minee(tooth.extraction))
            this->tooth = ToothTextureHint::extr_m;

        else
            this->tooth = ToothTextureHint::extr;
    }
    else if (tooth.implant.exists())
    {
        if (R_U_Minee(tooth.implant))
            this->tooth = ToothTextureHint::impl_m;
        else
            this->tooth = ToothTextureHint::impl;
    }
    else if (tooth.root.exists())
    {
        this->tooth = ToothTextureHint::root;
    }


    //surface hint

    for (int i = 0; i < surfaces.size(); i++)
    {
        surfaces[i] = SurfaceHint{ SurfaceColor::none, false };

        if (tooth.obturation.exists(i) && tooth.caries.exists(i))
        {
            surfaces[i].outline = true;
        }

        if (tooth.obturation.exists(i))
        {
            if (R_U_Minee(tooth.obturation[i]))
                surfaces[i].color = SurfaceColor::green;
            else
                surfaces[i].color = SurfaceColor::blue;
        }
        else if (tooth.caries.exists(i))
            surfaces[i].color = SurfaceColor::red;
    }

    //endo hint

    endo = EndoHint::none;

    if (tooth.endo.exists())
    {
        if (R_U_Minee(tooth.endo)) endo = EndoHint::green;
        else endo = EndoHint::blue;
    }
    else if (tooth.pulpitis.exists())
        endo = EndoHint::red;
    else if (tooth.lesion.exists() && !tooth.implant.exists())
        endo = EndoHint::darkred;

    //prosthodontic hint

    prostho = ProsthoHint::none;

    if (tooth.crown.exists())
    {
        R_U_Minee(tooth.crown) ?
            prostho = ProsthoHint::crown_green
            :
            prostho = ProsthoHint::crown;
    }
    else if (tooth.bridge.exists())
    {
        R_U_Minee(tooth.bridge) ?
            prostho = ProsthoHint::bridge_green
            :
            prostho = ProsthoHint::bridge;

        bridgePos = getBridgePosition(tooth.index, tooth.bridge.position);
    }
    else if (tooth.splint.exists())
    {
        R_U_Minee(tooth.splint) ?
            prostho = ProsthoHint::splint_green
            :
            prostho = ProsthoHint::splint;

        bridgePos = getBridgePosition(tooth.index, tooth.splint.position);

    }

    else if (tooth.denture.exists())
    {
        prostho = ProsthoHint::denture;

        if (!impacted &&
            this->tooth != ToothTextureHint::root &&
            this->tooth != ToothTextureHint::impl &&
            this->tooth != ToothTextureHint::impl_m
          ) this->tooth = ToothTextureHint::none;

    }

    post = PostHint::none;

    if (tooth.post.exists())
    {
        R_U_Minee(tooth.post) ?
            post = PostHint::green
            :
            post = PostHint::blue;
    }

    if (tooth.dsn)
    {
        dsn = std::make_unique<ToothPaintHint>(tooth.dsn.tooth());
        dsn->isHyperdontic = true;
        dsn->num = ToothUtils::getNhifNumber(dsn->idx, dsn->temp, true);
    }

    toolTip = "<b>" + tooth.toothName() + "</b><br>";

    if (notes.size()) {
        toolTip += "<br><b>Бележки:</b><br>" + notes;
    }
}
