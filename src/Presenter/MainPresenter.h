#pragma once

#include <vector>
#include "View/IMainView.h"
#include "TabPresenter/TabPresenter.h"
#include "ListSelector/ListSelectorPresenter.h"
#include "MonthNotifLoader.h"

class MainPresenter
{
	TabPresenter _tabPresenter;
	ListSelectorPresenter listSelector_;
	MonthNotifLoader m_notifLoader;
	IMainView* view;

public:
	MainPresenter();

	void setView(IMainView* view);
	void printPressed();
	void newAmbPressed();
	void newPerioPressed();
	void showListSelector();
	void generateReport();
	void generateInvoice();
	void settingsPressed();
	bool save();
	bool saveAs();
	void logOut();
	void userSettingsPressed();
	

	//returns false when user selects cancel, otherwise returns true
	//bool closeTab();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();
};

