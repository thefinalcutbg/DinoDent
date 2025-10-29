#pragma once

#include "Network/HIS/UpdateMedications.h"
#include "Presenter/ProcedureListPresenter.h"
#include "Network/HIS/EDental.h"
#include "Model/UserStructs.h"

class SettingsDialog;

class SettingsMainPresenter
{
	std::vector<PracticeDoctor> m_doctorsList;

	int m_currentIndex{ -1 };

	UpdateMedications med_update;
	EDental::Fetch his_fetch_service;

	SettingsDialog* view{nullptr};

	ProcedureListPresenter procedure_list;

	bool monthlySheets = false;

	void setUpdateLabels();
	static void importToDb(const AmbList& amb, const Patient& p);

public:

	SettingsMainPresenter();

	void setView(SettingsDialog* view);
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
	void priceUpdated(const std::string& code, double price);
	void hisImport();

};
