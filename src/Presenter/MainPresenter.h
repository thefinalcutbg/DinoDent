#pragma once

#include <vector>
#include "View/Interfaces/IMainView.h"
#include "TabPresenter.h"
#include "BrowserPresenter.h"
#include "Network/HIS/UpdateMedications.h"
#include "Network/HIS/EPrescription.h"
class MainPresenter
{
	IMainView* view{nullptr};

	UpdateMedications med_update_service;
	EPrescription::eRxFetch eRx_service;
	static MainPresenter s_singleton;

	MainPresenter(){}

	bool firstTimeLogic(); //returns false if there are no practices

public:

	static MainPresenter& get() { return s_singleton; }

	void setView(IMainView* view);
	void printPressed();
	void newAmbPressed();
	void newPerioPressed();
	void newPrescriptionPressed();
	void showBrowser();
	void pisDialog();
	void statisticPressed();
	void settingsPressed();
	void newInvoicePressed();
	bool save();
	void logOut();
	void userSettingsPressed();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();


};

