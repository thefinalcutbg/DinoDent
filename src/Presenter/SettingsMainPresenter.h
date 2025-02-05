#pragma once

#include "View/Interfaces/ISettingsDialog.h"
#include "Network/HIS/UpdateMedications.h"
#include "Presenter/ProcedureListPresenter.h"
#include "Network/HIS/EDental.h"

class SettingsMainPresenter
{
	std::vector<PracticeDoctor> m_doctorsList;

	int m_currentIndex{ -1 };

	UpdateMedications med_update;
	EDental::Fetch his_fetch_service;

	ISettingsDialog* view{nullptr};

	ProcedureListPresenter procedure_list;

	bool nhif_contract_temp = false;

	void setUpdateLabels();
	static void importToDb(const AmbList& amb, const Patient& p);

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);
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
