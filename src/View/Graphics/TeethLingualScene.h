#pragma once
#include <QGraphicsScene>
#include "Model/TableStructs.h"
#include "View/Graphics/PaintHint.h"

class ToothGraphicsItem;
class PerioChartItem;
class DsnToothGraphicsItem;
class SelectionBox;

class TeethLingualScene : public QGraphicsScene
{
	Q_OBJECT

	ToothGraphicsItem* toothGraphic[32];
	DsnToothGraphicsItem* dsnToothGraphic[32];
	SelectionBox* selectionBox[32];
	PerioChartItem* maxillaryChart;
	PerioChartItem* mandibularChart;

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:
	TeethLingualScene();
	void display(const ToothPaintHint& tooth);
	void setProcedures(std::vector<int> teeth_procedures);
	void setMeasurments(const int pd[192], const int cal[192]);
	void showPerio(bool shown);
	int selectedTooth();
	//Doesn't emit signal!
	void setSelectedTooth(int toothIdx);
};

