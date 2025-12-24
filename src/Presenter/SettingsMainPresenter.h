#pragma once

#include "Network/HIS/UpdateMedications.h"
#include "Presenter/ProcedureListPresenter.h"
#include "Network/HIS/EDental.h"
#include "Model/UserStructs.h"
#include "Network/SMS/Mobica.h"

class SettingsDialog;

class SettingsMainPresenter
{
	std::vector<PracticeDoctor> m_doctorsList;

	int m_currentIndex{ -1 };

	UpdateMedications med_update;
	EDental::Fetch his_fetch_service;
	Mobica::BalanceReplyHandler sms_balance_service;

	SettingsDialog* view{nullptr};

	ProcedureListPresenter procedure_list;

	bool monthlySheets = false;

	void setUpdateLabels();
	static void importToDb(const AmbList& amb, const Patient& p);

public:

	SettingsMainPresenter();

	void setView(SettingsDialog* view);
    void priceEditRequested(const std::string& code);
	void updateMedications();
	void addDoctor();
	void removeDoctor();
	void practiceDoctorChanged(int specialtyIdx, bool admin);
	void doctorIndexChanged(int index);
	void okPressed();
	bool applyChanges();
	void sqlCommandExec(const std::string& sql);
	void printEmptyDocs();
    void practiceTabFocused(); //need to refresh the doctor list
	void hisImport();
	void balanceRequested(const std::string& usr, const std::string& pass);

};
