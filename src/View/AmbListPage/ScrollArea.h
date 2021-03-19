#pragma once

#include <QScrollArea>
#include <QScrollBar>
#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <vector>

struct ScrollPos 
{ 
	int x; 
	int y; 
	ScrollPos(int x, int y) : x(x), y(y) {};
};

class ScrollArea : public QScrollArea
{
		std::vector<ScrollPos> tabsScrollPos;
		int currentPos;
		bool tabRemoved;
	
		

public:
	ScrollArea(QWidget *parent);
	
	void addTab();
	void removeTab(int vecPos);
	void tabChanged(int vecPos);

	~ScrollArea();
};
