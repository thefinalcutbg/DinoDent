#pragma once
#include <QGraphicsScene>
#include "Model/TableStructs.h"
#include "View/Graphics/PaintHint.h"

class ToothGraphicsItem;
class PerioChartItem;
class SelectionBox;

class TeethBuccalScene : public QGraphicsScene
{
	Q_OBJECT

	ToothGraphicsItem* toothGraphic[32];
	ToothGraphicsItem* dsnToothGraphic[32];
	SelectionBox* selectionBox[32];
	PerioChartItem* maxillaryChart;
	PerioChartItem* mandibularChart;

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
public:

	TeethBuccalScene();
	void display(const ToothPaintHint& tooth);
	void setProcedures(std::vector<int> teeth_procedures);
	void setMeasurments(const int pd[192], const int cal[192]);
	void showPerio(bool shown);

	int selectedTooth();
	//doesnt emit signal!
	void setSelectedTooth(int toothIdx);
};

