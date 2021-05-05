#pragma once
#include <QPainter>
#include <array>
#include <map>
#include "Model/Tooth/Tooth.h"
#include "SpriteRect.h"
#include "SpriteSheets.h"
#include "Model/Tooth/ToothUtils.h"
#include "PaintHint.h"


class ToothPainter
{
protected:

	QPixmap* currentTexture;
	QPixmap* commonTexture;
    const SpritesheetCoords* coords;

	
	ToothUtils utilities;
	//SpriteSheets spriteContainer;
	SpritesheetCoords molarCoordinates;
	SpritesheetCoords premolarCoordinates;
	SpritesheetCoords frontalCoordinates;

	const Tooth* tooth;
	
	int currentIndex;

	//do not copy this class, tooth_type is mapped to the allocated coordinates on initialization
	std::map<int, SpritesheetCoords*> tooth_type;

	
	void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex);

	QPixmap textureFormat(QRect crop, QColor color, double opacity); //paints the texture in specific colour and opacity
	QPixmap textureFormat(QRect crop, double opacity); //paints a texture with some opacity
	QPixmap textureFormat(QRect crop); //leaves the texture untouched

	QPixmap toothPixmap();
	QPixmap lesionPixmap();
	QPixmap endoPixmap();
	QPixmap perioPixmap();
	QPixmap surfacePixmap();
	QPixmap hyperdontic();
	QPixmap mobilityPaint();
	QPixmap toothNumber();
	
	virtual QPixmap paintToothStatus();
	
public:
	ToothPainter();


	virtual QPixmap* getPixmap(const Tooth& tooth);
	virtual  ~ToothPainter();
};

