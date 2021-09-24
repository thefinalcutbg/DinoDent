#pragma once
#include <QPainter>
#include <QGraphicsPixmapItem>

#include "SpriteRect.h"
#include "SpriteSheets.h"
#include "PaintHint.h"



class ToothPainter
{

protected:
	const ToothSprite* currentTexture;
	const SpritesheetCoords* coords;

	//SpriteSheets spriteContainer;
	SpritesheetCoords molarCoordinates;
	SpritesheetCoords premolarCoordinates;
	SpritesheetCoords frontalCoordinates;
	SpritesheetCoords temp_frontal;
	SpritesheetCoords temp_molar;

	std::map<int, SpritesheetCoords*> tooth_type;
	std::map<int, SpritesheetCoords*> temp_tooth_type;

	QPixmap textureFormat(const QPixmap& px, QColor color, double opacity); //paints the texture in specific colour and opacity
	QPixmap textureFormat(const QPixmap& px, double opacity); //paints a texture with some opacity
	QPixmap textureOutline(const QPixmap& src, QColor borderColor);

	QPixmap drawSurfaces(const ToothPaintHint& tooth);
	QPixmap mobilityPaint(const ToothPaintHint& tooth);
	QPixmap fracturePaint(const ToothPaintHint& tooth);
	QPixmap toothNumber(const ToothPaintHint& tooth);
	void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex);

	virtual QPixmap returnPaintedTooth(const ToothPaintHint& tooth);

public:
	ToothPainter();
	bool showLingual;
	virtual QPixmap* paintTooth(const ToothPaintHint& tooth);
	QPixmap getPixmap(const ToothPaintHint& tooth);
};

