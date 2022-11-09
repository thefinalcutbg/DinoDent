#pragma once

#include <vector>
#include "View/Interfaces/IMainView.h"
#include "TabPresenter.h"
#include "ListSelectorPresenter.h"
#include "MonthNotifPresenter.h"

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
	void newPerscriptionPressed();
	void showListSelector();
	void pisDialog();
	void settingsPressed();
	bool save();
	void logOut();
	void userSettingsPressed();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();
};

