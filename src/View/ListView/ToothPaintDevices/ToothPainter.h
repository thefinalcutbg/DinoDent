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
	SpritesheetCoords temp_frontal;
	SpritesheetCoords temp_molar;

	std::map<int, SpritesheetCoords*> tooth_type;
	std::map<int, SpritesheetCoords*> temp_tooth_type;

	QPixmap textureFormat(QRect crop, QColor color, double opacity); //paints the texture in specific colour and opacity
	QPixmap textureFormat(QRect crop, double opacity); //paints a texture with some opacity
	QPixmap textureFormat(QRect crop); //leaves the texture untouched
	QPixmap textureOutline(const QPixmap& src, QColor borderColor);

	QPixmap drawSurfaces(const ToothPaintHint& tooth);
	QPixmap mobilityPaint(const ToothPaintHint& tooth);
	QPixmap fracturePaint(const ToothPaintHint& tooth);
	QPixmap toothNumber(const ToothPaintHint& tooth);
	void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex);

	virtual QPixmap returnPaintedTooth(const ToothPaintHint& tooth);

public:
	ToothPainter();
	virtual QPixmap* paintTooth(const ToothPaintHint& tooth);
	
};

