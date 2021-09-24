#pragma once

#include <QGraphicsScene>
#include "View/ToothPaintDevices/ToothPainter.h"

class ToothGraphicsItem;
class ToothPaintHint;

class PerioScene : public QGraphicsScene
{
	ToothPainter toothPainter;
	ToothGraphicsItem* toothGraphic[16];

public:
	PerioScene();
	void display(const ToothPaintHint& tooth);

	~PerioScene();
};
