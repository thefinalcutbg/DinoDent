#pragma once
#include <QPixmap>
#include <unordered_map>

class SpriteSheets
{
	static SpriteSheets instance;

	SpriteSheets();

	QPixmap* maxTexture[8]; //the textures of the maxillary permanent teeth;
	QPixmap* mandTexture[8]; //the textures of the mandibular permanent teeth;
	QPixmap* maxDTexture[5]; //the textures of the maxillary deciduous teeth;
	QPixmap* mandDTexture[5]; //the textures of the mandibular deciduous teeth;
	QPixmap* commonTexture;
	QPixmap* permaTexture[32]; //textures of permanent teeth sorted by index;
	QPixmap* deciTexture[20];
	QPixmap* bridgeU;
	QPixmap* bridgeL;

	std::unordered_map<int, int> permaToTemp_map;
	
	void loadTextures();

public:

	SpriteSheets(const SpriteSheets&) = delete;

	~SpriteSheets();
	static SpriteSheets& container();
	static void LoadSpriteSheets();
	QPixmap* getTexture(int toothIndex, bool temporary);
	QPixmap* getCommonTexture();
	QPixmap* getUpperBridge();
	QPixmap* getLowerBridge();
};

