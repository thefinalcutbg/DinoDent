#pragma once

#include <QPixmap>
#include <unordered_map>
#include "ToothSprite.h"
#include "SpriteRect.h"
#include "Model/Dental/ToothUtils.h"

class SpriteSheets
{

	SpriteSheets();


	SpritesheetCoords molarRect;
	SpritesheetCoords premolarRect;
	SpritesheetCoords frontalRect;
	SpritesheetCoords tempFrontalRect;
	SpritesheetCoords tempMolarsRect;

	std::array<const SpritesheetCoords*, 32> permanentCoords;
	std::array<const SpritesheetCoords*, 32> tempoCoords;


	QPixmap* implantFront;
	QPixmap* calculusImplantFront;
	QPixmap* perioImplantFront;
	QPixmap* dentureFront;
	QPixmap* implantMolar;
	QPixmap* calculusImplantMolar;
	QPixmap* perioImplantMolar;
	QPixmap* dentureMolar;

	TexturePack textures[26];

public:
	SpriteSheets(const SpriteSheets&) = delete;
	SpriteSheets(SpriteSheets&&) = delete;
	SpriteSheets& operator=(SpriteSheets&&) = delete;
	SpriteSheets& operator=(const SpriteSheets&) = delete;

	static SpriteSheets& container() { static SpriteSheets instance; return instance; }
	const SpritesheetCoords& getCoordinates(int toothIndex, bool temporary);

	void initialize();

	const TexturePack& getTexturePack(int toothIdx, bool temp = false);




	~SpriteSheets();


};

