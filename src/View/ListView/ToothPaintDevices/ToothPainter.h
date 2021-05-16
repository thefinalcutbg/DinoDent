#pragma once
#include <QPainter>
#include <QGraphicsPixmapItem>

#include "SpriteRect.h"
#include "SpriteSheets.h"
#include "PaintHint.h"



class ToothPainter
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
	QPixmap textureOutline(const QPixmap& src, QColor borderColor);

	QPixmap drawSurfaces(const PaintHint& tooth);
	QPixmap mobilityPaint(const PaintHint& tooth);
	QPixmap toothNumber(const PaintHint& tooth);
	void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex);

	virtual QPixmap returnPaintedTooth(const PaintHint& tooth);

public:
	ToothPainter();
	virtual QPixmap* paintTooth(const PaintHint& tooth);
	
};

