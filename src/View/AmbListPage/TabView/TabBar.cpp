#include "TabBar.h"

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
	setExpanding(false);
	setMovable(true);
	setTabsClosable(true);
	setElideMode(Qt::TextElideMode::ElideRight);


}

void TabBar::addNewTab(QString tabName, QVariant vecPos)
{
	addTab(tabName);
	setTabData(count()-1, vecPos);
}

void TabBar::closeTab(int index)
{
	int vecPos = tabData(index).toInt();

	for (int i = 0; i < count(); i++)
	{
		int currentVecPos = tabData(i).toInt();

		if (currentVecPos > vecPos)
		{
			setTabData(i, currentVecPos - 1);
		}
	}

	removeTab(index);
}

TabBar::~TabBar()
{
}
