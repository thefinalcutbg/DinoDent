#pragma once
#include <QPainter>
#include <array>
#include <map>

#include "SpriteRect.h"
#include "SpriteSheets.h"
#include "PaintHint.h"

class ToothPainter2
{

protected:
	QPixmap* currentTexture;
	QPixmap* commonTexture;
	const SpritesheetCoords* coords;

	//SpriteSheets spriteContainer;
	SpritesheetCoords molarCoordinates;
	SpritesheetCoords premolarCoordinates;
	SpritesheetCoords frontalCoordinates;

	std::map<int, SpritesheetCoords*> tooth_type;

	QPixmap textureFormat(QRect crop, QColor color, double opacity); //paints the texture in specific colour and opacity
	QPixmap textureFormat(QRect crop, double opacity); //paints a texture with some opacity
	QPixmap textureFormat(QRect crop); //leaves the texture untouched

	
	QPixmap mobilityPaint(const PaintHint& tooth);
	QPixmap toothNumber(const PaintHint& tooth);
	void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex);

	virtual QPixmap returnPaintedTooth(const PaintHint& tooth);

public:
	ToothPainter2();
	virtual QPixmap* paintTooth(const PaintHint& tooth);
	
};

