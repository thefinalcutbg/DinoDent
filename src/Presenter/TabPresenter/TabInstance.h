#pragma once

#include "View/AmbListPage/TabView/ITabView.h"


enum class TabType {AmbList, PerioList, PatientSummary};

class TabInstance
{

	ScrollPos m_scrollPos;

protected:
	ITabView* _tabView;

public:
	const TabType type;

	TabInstance(ITabView* tabView, TabType type) : _tabView(tabView), type(type) {  };

	bool edited{ false };


	virtual bool save() = 0;
	virtual bool saveAs() = 0;
	virtual bool close() = 0;
	virtual void print() = 0;
	virtual void setCurrent() = 0;
	virtual std::string getTabName() = 0;

protected:

	void setScrollPosition() {_tabView->setScrollPos(m_scrollPos);}


public:


	virtual void prepareSwitch()
	{
		m_scrollPos = _tabView->getScrollPos();
	};

	virtual ~TabInstance() {};

};