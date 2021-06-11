#pragma once

class TabPresenter;

class ITabView
{
public:
	virtual void newTab(int vecPos, std::string tabName) = 0;
	virtual void focusTab(int vecPos) = 0;
	virtual void removeCurrentTab() = 0;
	virtual void changeTabName(std::string tabName) = 0;
	virtual void setTabPresenter(TabPresenter* presenter) = 0;
};