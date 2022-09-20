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
		case ToothType::Molar:
			permanentCoords[i] = &molarRect;
			tempoCoords[i] = nullptr;
			break;
		case ToothType::Premolar:
			permanentCoords[i] = &premolarRect;
			tempoCoords[i] = &tempMolarsRect;
			break;
		case ToothType::Frontal:
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

	QPixmap occlusal{ ":/tooth/tooth_occlusal.png" };
	QPixmap approximal{ ":/tooth/tooth_approximal.png" };
	QPixmap buccal{ ":/tooth/tooth_buccal.png" };
	QPixmap lingual{ ":/tooth/tooth_lingual.png" };
	QPixmap cervical{ ":/tooth/tooth_cervical.png" };

	QPixmap bridgeCon{ ":/tooth/tooth_bridgeCon.png" };
	QPixmap bridgeSep{ ":/tooth/tooth_bridgeSep.png" };

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

	tx.post = new QPixmap(master.post.copy(crownRect));
	tx.crown = new QPixmap(master.crown.copy(crownRect));
	tx.fiberOptic = new QPixmap(master.fiberBridge.copy(crownRect));
	tx.bridgeConnected = new QPixmap(master.bridgeCon.copy(crownRect));
	tx.bridgeSeparated = new QPixmap(master.bridgeSep.copy(crownRect));

	tx.surfaces[Surface::Occlusal] = new QPixmap(master.occlusal.copy(crownRect));
	tx.surfaces[Surface::Medial] = new QPixmap(master.approximal.copy(xPos + width / 2, 0, width / 2, 440));
	tx.surfaces[Surface::Distal] = new QPixmap(master.approximal.copy(xPos, 0, width/2, 440));
	tx.surfaces[Surface::Buccal] = new QPixmap(master.buccal.copy(xPos, 0, width, 230));
	tx.surfaces[Surface::Lingual] = new QPixmap(master.lingual.copy(xPos, 0, width, 300));
	tx.surfaces[Surface::Cervical] = new QPixmap(master.cervical.copy(xPos, 0, width, 50));
}

const std::unordered_set<int> molarTextureSet{ 0,1,2,8,9,10 };

void SpriteSheets::initialize()
{
	SpriteMaster masterSprites;

	constexpr int molarWidth = 180;
	constexpr int commonWidth = 120;

	QPixmap commonTexture(":/tooth/tooth_common.png");
	implant = new QPixmap(commonTexture.copy(QRect(0, 0, 120, 860)));
	lesionImplant = new QPixmap(commonTexture.copy(QRect(120, 0, 120, 860)));
	perioImplant = new QPixmap(commonTexture.copy(QRect(240, 0, 120, 860)));

	int xPos = 0;

	for (int i = 0; i < 26; i++)
	{
		int width = molarTextureSet.count(i) ? molarWidth : commonWidth;

		initializePack(textures[i], masterSprites, xPos, width);
		textures[i].implant = implant;
		textures[i].lesionImplant = lesionImplant;
		textures[i].perioImplant = perioImplant;

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
	delete implant;
	delete lesionImplant;
	delete perioImplant;
}
