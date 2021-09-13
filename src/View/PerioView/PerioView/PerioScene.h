#pragma once

#include <QGraphicsScene>

class ToothGraphicsItem;
class ToothPaintHint;

class PerioScene : public QGraphicsScene
{
	ToothGraphicsItem* toothGraphic[32];

public:
	PerioScene();
	void display(const ToothPaintHint& tooth);
	~PerioScene();
};
