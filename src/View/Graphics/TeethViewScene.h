#pragma once

#include <QGraphicsScene>

#include "Model/TableStructs.h"
#include "View/Graphics/PaintHint.h"

class ContextMenu;
class SelectionBox;
class BridgeItem;
class ToothGraphicsItem;
class ListPresenter;

class TeethViewScene : public QGraphicsScene
{
	Q_OBJECT


	ToothGraphicsItem* toothGraphic[32];
	ToothGraphicsItem* dsnToothGraphic[32];
	SelectionBox* selectionBox[32];

	ListPresenter* presenter;

	ContextMenu* contextMenu;


	void keyPressEvent(QKeyEvent* event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* Event) override;

public:

	TeethViewScene(QObject *parent = 0);
	void setContextMenu(ContextMenu* contextMenu);

	void setPresenter(ListPresenter* presenter);

	void display(const ToothPaintHint& tooth);
	void setSelectedTeeth(const std::vector<int> &selectedTeeth);
	void setProcedures(std::vector<int> teeth_procedures);
	void setNotes(const std::array<std::string, 32>& notes);

	~TeethViewScene();
};
