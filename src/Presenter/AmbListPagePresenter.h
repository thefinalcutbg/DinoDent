#pragma once

#include <vector>

#include "View/AmbListPage/IAmbListPage.h"
#include "PatientDialog/PatientDialogPresenter.h"
#include "TabPresenter/ListInstance.h"
#include "Database/DbAmbList.h"
#include "TabPresenter/TabPresenter.h"

class AmbListPagePresenter : public PatientDialogRequestor
{
	TabPresenter _tabPresenter;

	IAmbListPage* view;
	PatientDialogPresenter* patientDialog;
	ListPresenter* listPresenter;
	std::vector<ListInstance> list_instance;
	DbAmbList database;


	int currentVecPos;


public:
	AmbListPagePresenter( 
						IAmbListPage* AmbListPage, 
						PatientDialogPresenter* patientDialog,
						ListPresenter* listView);


	void newPressed();



	bool save();
	bool saveAs();
	bool closeTab();


	//as a requestor:
	virtual void setPatient(Patient patient);
};

