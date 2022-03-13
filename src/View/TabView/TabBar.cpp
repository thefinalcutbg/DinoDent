#include "TabBar.h"
#include <QMouseEvent>
#include <QLabel>
#include "TabTitle.h"
#include "View/Theme.h"




TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
	setExpanding(false);
	setMovable(true);
	setTabsClosable(true);
	setElideMode(Qt::TextElideMode::ElideNone);
	setObjectName("TabBar");

	setStyleSheet(
		"TabBar::tab{"
		"background-color:" + Theme::getRGBStringFromColor(Theme::inactiveTabBG) +
		"border-top-left-radius: 8px;"
		"border-top-right-radius: 8px;"
		"margin-right: 1px;"
		"}"

		"TabBar::tab:selected {"
		"background-color: " + Theme::getRGBStringFromColor(Theme::background) +
		"}"

		"QTabBar::tab:hover:!selected {"
		"background-color:" + Theme::getRGBStringFromColor(Theme::inactiveTabBGHover)+
			"}"
	);

	connect(this, &QTabBar::currentChanged,
		[=](int index) {
			changeActiveTabAppearence(index);
		});
}


void TabBar::changeActiveTabAppearence(int index)
{
	auto tabTitle = static_cast<TabTitle*>(tabButton(m_currentIdx, QTabBar::RightSide));

	if (tabTitle != nullptr)
		tabTitle->setCurrentAppearence(false);


	if (index != -1) {
		static_cast<TabTitle*>
			(tabButton(index, QTabBar::RightSide))
			->setCurrentAppearence(true);
	}

	m_currentIdx = index;

}

void TabBar::requestClose(QWidget* titleWidget)
{
	for (int i = 0; i < count(); i++)
	{
		if (tabButton(i, TabBar::ButtonPosition::RightSide) == static_cast<QWidget*>(titleWidget))
		{
			emit tabCloseRequested(i);
		}
	}
}

void TabBar::addNewTab(int mapIndex, const QString& header, const QString& footer)
{
	QSignalBlocker blocker(this);

	TabTitle* tab = new TabTitle(this, mapIndex);
	tab->setText(header, footer);

	changeActiveTabAppearence(count() - 1); //because we blocked the signal!
}

void TabBar::changeTabName(int tabIndex, const QString& header, const QString& footer)
{
	static_cast<TabTitle*>
		(tabButton(tabIndex, QTabBar::ButtonPosition::RightSide))
		->setText(header, footer);
}

int TabBar::getTabId(int tabIndex)
{
	if (tabIndex < 0) return -1;

	return static_cast<TabTitle*>
		(tabButton(tabIndex, QTabBar::RightSide))->tabId;
}

void TabBar::closeTab(int index)
{
	removeTab(index);
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		int tab = tabAt(event->pos());

		if (tab != -1)
			tabCloseRequested(tab);
	}

	QTabBar::mouseReleaseEvent(event);
}


TabBar::~TabBar()
{
}
