#pragma once

#include <vector>

#include "Model/ListInstance.h"

#include "View/ListView/IListView.h"

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

#include "BridgeController.h"
#include "StatusControl.h"
#include "CheckModelCreator.h"
#include "SelectionManager.h"

class EditObserver
{
public:
	virtual void notify() = 0;
};

class ListPresenter : 
	public PatientDialogRequestor, 
	public AllergiesDialogRequestor,
	public IStatusControl
{

	IListView* view;
	PatientDialogPresenter *patientDialog;
	AllergiesDialogPresenter* allergiesDialog;
	ProcedureDialogPresenter* procedureDialog;

	EditObserver* edit_observer;

	SelectionManager selectionManager;
	CheckModelCreator checkModelCreator;
	StatusControl statusControl;
	BridgeController bridgeController;

	AmbList* ambList;
	Patient* patient;
	std::vector<int>* selectedIndexes;
	bool* edited;

	void makeEdited();
	void statusChanged();

public:
	ListPresenter(IListView* view);

	void setDialogPresnters(
							AllergiesDialogPresenter* allergiesPresenter,
							ProcedureDialogPresenter* procedurePresenter);

	void setPatientDialog(PatientDialogPresenter* presenter);

	void setData(ListInstance* listInstance);

	void attachEditObserver(EditObserver* observer); //changes the asterik on tab name when edited;

	//setting the indexes of the selected teeth and constructing vector of pointers them:
	void setSelectedTeeth(std::vector<int>& selectedTeethIndex);

	//status changer:
	void changeStatus(Surface surface, SurfaceType statusType);
	void changeStatus(StatusAction status);


	void openPatientDialog();
	void openAllergiesDialog();
	void openProcedureDialog();
	void statusChanged(std::string status);

	// Inherited via PatientDialogRequestor
	void setPatient(Patient patient) override;
	void setAllergies(Allergies allergies) override;

};

