#pragma once

#include <QGraphicsScene>
#include "View/Graphics/PaintHint.h"

class ContextMenu;
class SelectionBox;
class BridgeItem;
class ToothGraphicsItem;
class SimpleToothItem;
class ListPresenter;
class SelectableGraphicsItem;
class QGraphicsTextItem;

class TeethViewScene : public QGraphicsScene
{
	Q_OBJECT


	ToothGraphicsItem* toothGraphic[32];
	ToothGraphicsItem* dsnToothGraphic[32];
	SelectableGraphicsItem* selectableItems[32]{ nullptr };
	SelectionBox* selectionBox[32];
	SimpleToothItem* simpleTooth[32];
	QGraphicsTextItem* title;
	QGraphicsTextItem* legend[3]{ nullptr };

	ListPresenter* presenter;

	bool m_simple_view = false;

	ContextMenu* contextMenu;
    //WORKS ONLY ON PHONETIC
    static int keyCodeMapper(QKeyEvent* e);

    void keyPressEvent(QKeyEvent* event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* Event) override;
	void initStatusLegend();
public:

	TeethViewScene(QObject *parent = 0);
	void setContextMenu(ContextMenu* contextMenu);

	void setPresenter(ListPresenter* presenter);
    std::vector<int> getSelectedTeethIdx();
	void display(const ToothPaintHint& tooth);
	void setSelectedTeeth(const std::vector<int> &selectedTeeth);
	void setProcedures(std::vector<int> teeth_procedures);
	void setNotes(const std::array<std::string, 32>& notes);
    void drawFocused(bool focused);
    
	void setSimpleView(bool simple);
	bool isSimpleView() { return m_simple_view; }
	~TeethViewScene();

signals:
    void returnPressed();
	void toothDoubleClicked(int idx);
};
