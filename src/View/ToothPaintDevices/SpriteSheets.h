#pragma once
#include <QPixmap>
#include <unordered_map>
#include "ToothSprite.h"
#include "SpriteRect.h"
//nanovo trqq se napi6e, da se nacepqt teksturite predvaritelno

class SpriteSheets
{
	static SpriteSheets instance;

	SpritesheetCoords molarRect;
	SpritesheetCoords premolarRect;
	SpritesheetCoords frontalRect;
	SpritesheetCoords tempFrontalRect;
	SpritesheetCoords tempMolarsRect;

	SpriteSheets();


	QPixmap* implant;
	QPixmap* lesionImplant;
	QPixmap* perioImplant;

	ToothSprite* permaTexture[32]; //textures of permanent teeth sorted by index;
	ToothSprite* deciTexture[20];

	std::array<ToothSprite, 8> maxPermanentSprites;
	std::array<ToothSprite, 8> mandPermanentSprites;
	std::array<ToothSprite, 5> maxTemporarySprites;
	std::array<ToothSprite, 5> mandTemporarySprites;

	QPixmap* bridgeU;
	QPixmap* bridgeL;

	std::unordered_map<int, int> permaToTemp_map;
	
	int count{ 0 };
	

	QPixmap* textureCut(const QPixmap& spriteSheet, QRect rect);
	QPixmap* textureCut(const QPixmap& spriteSheet, QRect rect, QRect erase);

public:

	SpriteSheets(const SpriteSheets&) = delete;

	~SpriteSheets();
	static SpriteSheets& container();
	void loadTextures();
	const ToothSprite& getTexture(int toothIndex, bool temporary);
	QPixmap* getUpperBridge();
	QPixmap* getLowerBridge();
	QPixmap getRawBridge(int tooth_idx);
};

