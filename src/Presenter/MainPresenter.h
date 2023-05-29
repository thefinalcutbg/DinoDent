#pragma once

#include <vector>
#include "View/Interfaces/IMainView.h"
#include "TabPresenter.h"
#include "BrowserPresenter.h"
#include "MonthNotifPresenter.h"
#include "Network/HIS/UpdateMedications.h"

class MainPresenter
{
	BrowserPresenter m_browserPresenter;
	MonthNotifPresenter m_notifPresenter;

	IMainView* view{nullptr};

	UpdateMedications med_update_service;

	static MainPresenter s_singleton;

	MainPresenter(){}

public:

	static MainPresenter get() { return s_singleton; }

	void setView(IMainView* view);
	void printPressed();
	void newAmbPressed();
	void newPerioPressed();
	void newPerscriptionPressed();
	void showListSelector();
	void pisDialog();
	void statisticPressed();
	void settingsPressed();
	bool save();
	void logOut();
	void userSettingsPressed();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();


};

