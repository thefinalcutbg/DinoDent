#pragma once

#include <QGraphicsScene>
#include "../ToothPaintDevices/ToothPainter.h"
#include "../ToothPaintDevices/BridgePainter.h"
#include "Model/Procedure/TableStructs.h"

class ContextMenu;
class SelectionBox;
class BridgeItem;
class ToothGraphicsItem;
class StatusPresenter;

class TeethViewScene : public QGraphicsScene
{
	Q_OBJECT


	ToothGraphicsItem* toothGraphic[32];
	SelectionBox* selectionBox[32];

	BridgeItem* upperBridge;
	BridgeItem* lowerBridge;

	BridgePainter bridgePainter;
	ToothPainter toothPainter;

	StatusPresenter* presenter;

	ContextMenu* contextMenu;


	void keyPressEvent(QKeyEvent* event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* Event) override;

public:

	TeethViewScene(QObject *parent = 0);
	void setContextMenu(ContextMenu* contextMenu);

	void setPresenter(StatusPresenter* presenter);

	void display(ToothPaintHint tooth);
	void display(const BridgesPaintHint& bridges);
	void setSelectedTeeth(const std::vector<int> &selectedTeeth);
	void setProcedures(std::vector<ProcedureRowData> procedures);

	~TeethViewScene();
};
