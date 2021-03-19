#pragma once

#include <vector>

#include "View/AmbListPage/IAmbListPage.h"
#include "PatientDialog/PatientDialogPresenter.h"
#include "Model/ListInstance.h"
#include "Model/DbAmbList.h"

class AmbListPagePresenter : public PatientDialogRequestor, public EditObserver
{
	IAmbListPage* view;
	PatientDialogPresenter* patientDialog;
	ListPresenter* listView;
	std::vector<ListInstance> list_instance;
	DbAmbList database;

	int currentVecPos;
	ListInstance* currentListInstance;

public:
	AmbListPagePresenter(Database* database, 
						IAmbListPage* AmbListPage, 
						PatientDialogPresenter* patientDialog,
						ListPresenter* listView);


	void newPressed();
	void tabChanged(int vecPos);  
	
	void notify();


	bool save();
	bool saveAs();
	bool closeTab();


	//as a requestor:
	virtual void setPatient(Patient patient);
};

