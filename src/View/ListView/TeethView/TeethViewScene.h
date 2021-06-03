#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "../ToothPaintDevices/ToothPainter.h"
#include "../ToothPaintDevices/BridgePainter.h"

#include "ToothGraphicsItem.h"
#include "SelectionBox.h"
#include "BridgeItem.h"
#include "ContextMenu.h"


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

public:

	TeethViewScene(QObject *parent = 0);
	void setContextMenu(ContextMenu* contextMenu);

	void setPresenter(StatusPresenter* presenter);

	void display(ToothPaintHint tooth);
	void display(const BridgesPaintHint& bridges);
	void setSelectedTeeth(std::vector<int> &selectedTeeth);

	~TeethViewScene();
};
