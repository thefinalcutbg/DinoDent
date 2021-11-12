#include "TabBar.h"
#include <QMouseEvent>

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		int tab = tabAt(event->pos());

		if(tab != -1)
			tabCloseRequested(tab);
	}

	QTabBar::mouseReleaseEvent(event);
}

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
	setExpanding(false);
	setMovable(true);
	setTabsClosable(true);
	setElideMode(Qt::TextElideMode::ElideRight);
}

void TabBar::addNewTab(QString tabName, QVariant mapIndex)
{
	QSignalBlocker blocker(this);
	addTab(tabName);
	setTabData(count()-1, mapIndex);
}

void TabBar::closeTab(int index)
{
	removeTab(index);
}

TabBar::~TabBar()
{
}
