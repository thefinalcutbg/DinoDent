#pragma once
#include <QGraphicsScene>
#include "Model/Procedure/TableStructs.h"
#include "View/ToothPaintDevices/PaintHint.h"


class ToothGraphicsItem;
class PerioChartItem;

class TeethBuccalScene : public QGraphicsScene
{
	Q_OBJECT

	ToothGraphicsItem* toothGraphic[32];
	PerioChartItem* maxillaryChart;
	PerioChartItem* mandibularChart;

public:
	TeethBuccalScene();
	void display(const ToothPaintHint& tooth);
	void setProcedures(std::vector<int> teeth_procedures);
	void setMeasurments(const int pd[192], const int cal[192]);
	void showPerio(bool shown);
};

