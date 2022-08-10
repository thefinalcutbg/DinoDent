#pragma once

#include <vector>
#include "View/IMainView.h"
#include "TabPresenter/TabPresenter.h"
#include "ListSelector/ListSelectorPresenter.h"
#include "PISDialogPresenter/MonthNotifPresenter/MonthNotifPresenter.h"

class MainPresenter
{
	TabPresenter m_tabPresenter;
	ListSelectorPresenter m_listSelector;
	MonthNotifPresenter m_notifPresenter{ &m_tabPresenter };
	IMainView* view{nullptr};

public:
	MainPresenter();

	void setView(IMainView* view);
	void printPressed();
	void newAmbPressed();
	void newPerioPressed();
	void showListSelector();
	void pisDialog();
	void settingsPressed();
	bool save();
	void logOut();
	void userSettingsPressed();
	

	//returns false when user selects cancel, otherwise returns true
	//bool closeTab();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();
};

