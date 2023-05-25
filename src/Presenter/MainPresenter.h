#pragma once

#include <vector>
#include "View/Interfaces/IMainView.h"
#include "TabPresenter.h"
#include "BrowserPresenter.h"
#include "MonthNotifPresenter.h"
#include "Network/HIS/UpdateMedications.h"

class MainPresenter
{
	TabPresenter m_tabPresenter;
	BrowserPresenter m_browserPresenter;
	MonthNotifPresenter m_notifPresenter{ &m_tabPresenter };
	IMainView* view{nullptr};

	UpdateMedications med_update_service;

public:
	MainPresenter();

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

