#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "../ToothPaintDevices/ToothPainter.h"
#include "../ToothPaintDevices/BridgePainter.h"
#include "Presenter/ListPresenter/ListPresenter.h"

#include "../ToothPaintDevices/ToothPainter2.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"

#include "ToothGraphicsItem.h"
#include "SelectionBox.h"
#include "BridgeItem.h"
#include "ContextMenu.h"

class TeethViewScene : public QGraphicsScene
{
	Q_OBJECT

	ToothGraphicsItem* toothGraphic[32];
	SelectionBox* selectionBox[32];

	BridgeItem* upperBridge;
	BridgeItem* lowerBridge;

	BridgePainter bridgePainter;
	ToothPainter toothPainter;

	ToothPainter2 toothPainter2;
	ToothHintCreator hint_creator;

	ListPresenter* presenter;

	ContextMenu* contextMenu;


	void keyPressEvent(QKeyEvent* event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

public:

	TeethViewScene(ListPresenter* presenter, QObject *parent = 0);
	void setContextMenu(ContextMenu* contextMenu);

	void display(Tooth tooth);
	void display(std::array<BridgeAppearance, 32> bridges);
	void setSelectedTeeth(std::vector<int> &selectedTeeth);

	~TeethViewScene();
};
