#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "ToothPaintHint.h"

Flag getMadeByUsrFlag(const DentistMade& status, Flag flag)
{
	std::string LPK("220008771"); //placeholder for LPK of the user
	if (status.LPK == LPK)
	{
		return Flag::green;
	}
	return flag;
}

ToothPaintHint ToothPaintConverter(const Tooth& tooth)
{

	ToothPaintHint texture;

	texture.index = tooth.index;
	texture.toothNum = ToothUtils::getToothNumber(tooth.index, tooth.temporary.exists());

	if (tooth.extraction.exists())
	{
		texture.tooth = Flag::none;

		if(!tooth.bridge.exists())
		{
			texture.extraction = getMadeByUsrFlag(tooth.extraction, Flag::normal);
		}
	}
	else if (tooth.implant.exists())
	{
		texture.tooth = Flag::none;
		texture.implant = getMadeByUsrFlag(tooth.implant, Flag::normal);
	}
	else if (tooth.root.exists())
	{
		texture.root = Flag::normal;
		texture.tooth = Flag::none;
	}

	//endo
	if (tooth.lesion.exists())
	{
		texture.lesion = Flag::normal;
		texture.endo = Flag::dakrRed;
	}

	if (tooth.pulpitis.exists())
	{
		texture.endo = Flag::red;
	}
	else if (tooth.endo.exists())
	{
		texture.endo = getMadeByUsrFlag(tooth.endo, Flag::blue);
	}

	if (tooth.post.exists())
	{
		texture.post = getMadeByUsrFlag(tooth.post, Flag::blue);
	}


	//surfaces:
	for (int i = 0; i < 6; i++)
	{
		if (tooth.obturation.exists(i) && tooth.caries.exists(i))
		{
			texture.surface[i] = Flag::normal;
		}
		else if (tooth.obturation.exists(i))
		{
			texture.surface[i] = getMadeByUsrFlag(tooth.obturation[i], Flag::blue);
		}
		else if (tooth.caries.exists(i))
		{
			texture.surface[i] = Flag::red;
		}
	}

	if (tooth.periodontitis.exists())
	{
		texture.paro = Flag::normal;
	}

	if (tooth.crown.exists())
	{
		texture.crown = getMadeByUsrFlag(tooth.crown, Flag::normal);
	}

	if (tooth.mobility.exists())
	{
		texture.mobility = static_cast<int>(tooth.mobility.degree) + 1; //0 is the default one
	}

};

