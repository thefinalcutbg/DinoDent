#include "ScrollArea.h"

ScrollArea::ScrollArea(QWidget *parent)
	: QScrollArea(parent),
	currentPos(0),
	tabRemoved(false)
{

}


void ScrollArea::addTab()
{
	QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	tabsScrollPos.push_back(ScrollPos(0, 0));
	tabRemoved = false;
}

void ScrollArea::removeTab(int vecPos)
{
	tabsScrollPos.erase(tabsScrollPos.begin() + vecPos);
	tabRemoved = true;
	
	if(!tabsScrollPos.size())
	{ 
		QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

}

void ScrollArea::tabChanged(int vecPos)
{
	if (currentPos < tabsScrollPos.size() && !tabRemoved)
	{
		tabsScrollPos[currentPos].x = horizontalScrollBar()->value();
		tabsScrollPos[currentPos].y = verticalScrollBar()->value();
		tabRemoved = false;
	}



	horizontalScrollBar()->setValue(tabsScrollPos[vecPos].x);
	verticalScrollBar()->setValue(tabsScrollPos[vecPos].y);
	currentPos = vecPos;
	
}

void ScrollArea::saveCurrentPosition()
{
	if (currentPos < tabsScrollPos.size() && !tabRemoved)
	{
		tabsScrollPos[currentPos].x = horizontalScrollBar()->value();
		tabsScrollPos[currentPos].y = verticalScrollBar()->value();
		tabRemoved = false;
	}
}

void ScrollArea::changePosition(int vecPos)
{
	horizontalScrollBar()->setValue(tabsScrollPos[vecPos].x);
	verticalScrollBar()->setValue(tabsScrollPos[vecPos].y);
	currentPos = vecPos;
}


ScrollArea::~ScrollArea()
{
}
