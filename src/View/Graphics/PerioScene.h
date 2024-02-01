#pragma once
#include <QGraphicsScene>

class ToothGraphicsItem;
struct ToothPaintHint;

class PerioScene : public QGraphicsScene
{
	ToothGraphicsItem* toothGraphic[16];
	ToothGraphicsItem* dsnToothGraphic[16];

public:
	PerioScene();
	void display(const ToothPaintHint& tooth);
	void deletePixmaps();

	~PerioScene();
};
