#pragma once

#include <vector>
#include "View/IMainView.h"
#include "Database/DbAmbList.h"
#include "TabPresenter/TabPresenter.h"
#include "ListSelector/ListSelectorPresenter.h"

class MainPresenter
{
	TabPresenter _tabPresenter;
	ListSelectorPresenter listSelector_;
	IMainView* view;

	DbAmbList amb_db;

public:
	MainPresenter();

	void setView(IMainView* view);
	void printPressed();
	void newAmbPressed();
	void newPerioPressed();
	void showListSelector();
	void generateReport();
	bool save();
	bool saveAs();
	void logOut();
	void userSettingsPressed();
	

	//returns false when user selects cancel, otherwise returns true
	bool closeTab();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();
};

