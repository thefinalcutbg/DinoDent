#pragma once

#include <QGraphicsItem>

class SelectableGraphicsItem : public QGraphicsObject
{
protected:
	int m_idx;
	bool m_focused = false;
	bool hovered = false;
public:

	SelectableGraphicsItem(int idx) : m_idx(idx)
	{
		setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
		setAcceptHoverEvents(true);
	}

	int getIndex() { return m_idx; }
	void drawFocused(bool focused) {
		m_focused = focused;
		update();
	}

};