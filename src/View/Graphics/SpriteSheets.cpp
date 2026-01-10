#include "SpriteSheets.h"
#include <unordered_set>

SpriteSheets::SpriteSheets()
	:
	molarRect(SpriteRectType::molar),
	premolarRect(SpriteRectType::premolar),
	frontalRect(SpriteRectType::frontal),
	tempFrontalRect(SpriteRectType::tempFrontal),
	tempMolarsRect(SpriteRectType::tempMolar)
{

	for (int i = 0; i < 32; i++)
	{
		switch (ToothUtils::getToothType(i))
		{
		case Dental::Type::Molar:
			permanentCoords[i] = &molarRect;
			tempoCoords[i] = nullptr;
			break;
		case Dental::Type::Premolar:
			permanentCoords[i] = &premolarRect;
			tempoCoords[i] = &tempMolarsRect;
			break;
		case Dental::Type::Frontal:
			permanentCoords[i] = &frontalRect;
			tempoCoords[i] = &tempFrontalRect;
			break;
		}
	}


}

struct SpriteMaster
{
	QPixmap teeth{ ":/tooth/tooth_teeth.png" };
	QPixmap roots{ ":/tooth/tooth_roots.png" };
	QPixmap lesion{ ":/tooth/tooth_lesion.png" };
	QPixmap endo{ ":/tooth/tooth_endo.png" };
	QPixmap crown{ ":/tooth/tooth_crown.png" };
	QPixmap fiberBridge{ ":/tooth/tooth_fiberBridge.png" };
	QPixmap perio{ ":/tooth/tooth_perio.png" };
	QPixmap post{ ":/tooth/tooth_post.png" };
	QPixmap calculus{ ":/tooth/tooth_calculus.png" };
	QPixmap occlusal{ ":/tooth/tooth_occlusal.png" };
	QPixmap approximal{ ":/tooth/tooth_approximal.png" };
	QPixmap buccal{ ":/tooth/tooth_buccal.png" };
	QPixmap lingual{ ":/tooth/tooth_lingual.png" };
	QPixmap cervical{ ":/tooth/tooth_cervical.png" };
	QPixmap resorption{ ":/tooth/tooth_resorption.png" };
	QPixmap bridgeCon{ ":/tooth/tooth_bridgeCon.png" };
	QPixmap bridgeSep{ ":/tooth/tooth_bridgeSep.png" };
	QPixmap falseTooth{ ":/tooth/tooth_false.png" };
};



constexpr int commonHeight = 860;

void initializePack(TexturePack& tx, const SpriteMaster master, int xPos, int width)
{
	QRect commonRect(xPos, 0, width, commonHeight);
	QRect crownRect(xPos, 0, width, 440);

	tx.tooth = new QPixmap(master.teeth.copy(commonRect));
	tx.endo = new QPixmap(master.endo.copy(commonRect));
	tx.lesion = new QPixmap(master.lesion.copy(commonRect));
	tx.root = new QPixmap(master.roots.copy(commonRect));
	tx.perio = new QPixmap(master.perio.copy(commonRect));
	tx.calculus = new QPixmap(master.calculus.copy(crownRect));
	tx.post = new QPixmap(master.post.copy(crownRect));
	tx.crown = new QPixmap(master.crown.copy(crownRect));
	tx.fiberOptic = new QPixmap(master.fiberBridge.copy(crownRect));
	tx.bridgeConnected = new QPixmap(master.bridgeCon.copy(crownRect));
	tx.bridgeSeparated = new QPixmap(master.bridgeSep.copy(crownRect));
	tx.falseTooth = new QPixmap(master.falseTooth.copy(crownRect));
	tx.resorption = new QPixmap(master.resorption.copy(commonRect));
	tx.surfaces[Dental::Surface::Occlusal] = new QPixmap(master.occlusal.copy(crownRect));
	tx.surfaces[Dental::Surface::Medial] = new QPixmap(master.approximal.copy(xPos + width / 2, 0, width / 2, 440));
	tx.surfaces[Dental::Surface::Distal] = new QPixmap(master.approximal.copy(xPos, 0, width/2, 440));
	tx.surfaces[Dental::Surface::Buccal] = new QPixmap(master.buccal.copy(xPos, 0, width, 230));
	tx.surfaces[Dental::Surface::Lingual] = new QPixmap(master.lingual.copy(xPos, 0, width, 300));
	tx.surfaces[Dental::Surface::Cervical] = new QPixmap(master.cervical.copy(xPos, 0, width, 50));
}

const std::unordered_set<int> molarTextureSet{ 0,1,2,8,9,10 };

void SpriteSheets::initialize()
{
	SpriteMaster masterSprites;

	constexpr int molarWidth = 180;
	constexpr int commonWidth = 120;

	QPixmap commonTexture(":/tooth/tooth_common.png");
	implantFront = new QPixmap(commonTexture.copy(QRect(0, 0, 120, 860)));
	calculusImplantFront = new QPixmap(commonTexture.copy(QRect(120, 0, 120, 860)));
	perioImplantFront = new QPixmap(commonTexture.copy(QRect(240, 0, 120, 860)));
	dentureFront = new QPixmap(commonTexture.copy(QRect(360, 0, 120, 860)));
	implantMolar = new QPixmap(commonTexture.copy(QRect(480, 0, 180, 860)));
	calculusImplantMolar = new QPixmap(commonTexture.copy(QRect(660, 0, 180, 860)));
	perioImplantMolar = new QPixmap(commonTexture.copy(QRect(840, 0, 180, 860)));
	dentureMolar = new QPixmap(commonTexture.copy(QRect(1020, 0, 180, 860)));

	int xPos = 0;

	for (int i = 0; i < 26; i++)
	{
		bool isMolar = molarTextureSet.count(i);

		int width = isMolar ? molarWidth : commonWidth;

		initializePack(textures[i], masterSprites, xPos, width);
		textures[i].implant = isMolar ? implantMolar : implantFront;
		textures[i].calculusImplant = isMolar ? calculusImplantMolar : calculusImplantFront;
		textures[i].perioImplant = isMolar ? perioImplantMolar : perioImplantFront;
		textures[i].denture = isMolar ? dentureMolar : dentureFront;

		xPos += width;
	}

}

constexpr int permaIdx[32]{
0,	1,	2,	3,	4,	5,	6,	7,		7,	6,	5,	4,	3,	2,	1,	0,
8,	9,	10,	11,	12,	13,	14,	15,		15,	14,	13,	12,	11,	10,	9,	8
};

constexpr int tempIdx[32]
{
	-1,	-1,	-1,	16,	17,	18,	19,	20,		20,	19,	18,	17,	16,	-1,	-1,	-1,
	-1,	-1,	-1,	21,	22,	23,	25,	25,		25,	24,	23,	22,	21,	-1,	-1,	-1
};

const TexturePack& SpriteSheets::getTexturePack(int toothIdx, bool temp)
{
	int textureIndex = temp ? tempIdx[toothIdx] : permaIdx[toothIdx];

	return textures[textureIndex];
}

const SpritesheetCoords& SpriteSheets::getCoordinates(int toothIndex, bool temporary)
{
	return temporary ? *tempoCoords[toothIndex] : *permanentCoords[toothIndex];
}


SpriteSheets::~SpriteSheets()
{
	delete implantFront;
	delete calculusImplantFront;
	delete perioImplantFront;
	delete dentureFront;
	delete implantMolar;
	delete calculusImplantMolar;
	delete perioImplantMolar;
	delete dentureMolar;
}
