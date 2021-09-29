#pragma once
#include <QPixmap>
#include <unordered_map>
#include "ToothSprite.h"
#include "SpriteRect.h"
#include "Model/Tooth/ToothUtils.h"

class SpriteSheets
{
	static SpriteSheets instance;

	SpriteSheets();

	SpritesheetCoords molarRect;
	SpritesheetCoords premolarRect;
	SpritesheetCoords frontalRect;
	SpritesheetCoords tempFrontalRect;
	SpritesheetCoords tempMolarsRect;
	
	std::array<const SpritesheetCoords*, 32> permanentCoords;
	std::array<const SpritesheetCoords*, 32> tempoCoords;

	QPixmap* implant{ nullptr };
	QPixmap* lesionImplant{ nullptr };
	QPixmap* perioImplant{ nullptr };

	TexturePack* permaTexture[32]{ nullptr }; //textures of permanent teeth sorted by index;
	TexturePack* deciTexture[20]{ nullptr };

	std::array<TexturePack, 8> maxPermanentSprites{ nullptr };
	std::array<TexturePack, 8> mandPermanentSprites{ nullptr };
	std::array<TexturePack, 5> maxTemporarySprites{ nullptr };
	std::array<TexturePack, 5> mandTemporarySprites{ nullptr };

	QPixmap* bridgeU{ nullptr };
	QPixmap* bridgeL{ nullptr };

	std::unordered_map<int, int> permaToTemp_map;

public:

	SpriteSheets(const SpriteSheets&) = delete;

	~SpriteSheets();
	static SpriteSheets& container();
	void loadTextures();
	const TexturePack& getTexturePack(int toothIndex, bool temporary);
	const SpritesheetCoords& getCoordinates(int toothIndex, bool temporary);
	QPixmap* getUpperBridge();
	QPixmap* getLowerBridge();
	QPixmap getRawBridge(int tooth_idx);
};

