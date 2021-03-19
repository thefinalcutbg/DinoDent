#include "ListPresenter.h"
#include "StatusToUIBridge.h"

ListPresenter::ListPresenter(IListView* view) :
	view(view),
	ambList(NULL),
	patient(NULL),
	selectedIndexes(NULL),
	patientDialog(NULL),
	edit_observer(NULL),
	allergiesDialog(NULL),
	edited(NULL),
	procedureDialog(NULL)
{
	checkModelCreator.addReciever(&statusControl);
	checkModelCreator.addReciever(view);
}

void ListPresenter::setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter, ProcedureDialogPresenter* procedurePresenter)
{
	allergiesDialog = allergiesPresenter;
	procedureDialog = procedurePresenter;
}

void ListPresenter::attachEditObserver(EditObserver* observer)
{
	edit_observer = observer;
}


void ListPresenter::setPatientDialog(PatientDialogPresenter* patientDialogPresenter)
{
	patientDialog = patientDialogPresenter;
}


void ListPresenter::setData(ListInstance* listInstance)
{
	this->ambList = &listInstance->amb_list;
	this->patient = &listInstance->patient;
	this->edited = &listInstance->edited;
	this->selectedIndexes = &listInstance->selectedIndexes;

	//bridgeController.setTeeth(&ambList->teeth);
	selectionManager.setStatus(&ambList->teeth);

	setSelectedTeeth(listInstance->selectedIndexes);

	view->refresh(*this->ambList, *this->patient, *selectedIndexes);
	view->repaintBridges(StatusToUIBridge(ambList->teeth));
}

void ListPresenter::setSelectedTeeth(std::vector<int>& SelectedIndexes)
{
	*this->selectedIndexes = SelectedIndexes;


	auto selectedTeeth = selectionManager.getSelectedTeethPointers(SelectedIndexes);
	statusControl.setSelectedTeeth(selectedTeeth);
	checkModelCreator.refreshModel(selectedTeeth);

	if (selectedIndexes->size() == 1) {
		view->updateControlPanel(&ambList->teeth[selectedIndexes->at(0)]);
	}
	else {
		view->updateControlPanel(NULL);
	}
}

void ListPresenter::makeEdited()
{
	if (*edited) return;
	*edited = true;
	if (edit_observer != NULL)
		edit_observer->notify();
}

void ListPresenter::statusChanged()
{
	makeEdited();

	checkModelCreator.refreshModel(selectionManager.getSelectedTeethPointers(*selectedIndexes));

	for (int i : *selectedIndexes)
	{
		view->repaintTooth(ambList->teeth[i]);
	}

	view->repaintBridges(StatusToUIBridge(ambList->teeth));

	if (selectedIndexes->size() == 1) {
		view->updateControlPanel(&ambList->teeth[selectedIndexes->at(0)]);
	}
	else {
		view->updateControlPanel(NULL);
	}
}


void ListPresenter::changeStatus(Surface surface, SurfaceType type)
{
	statusControl.changeStatus(surface, type);
	statusChanged();
}

void ListPresenter::changeStatus(StatusAction status)
{
	statusControl.changeStatus(status);

	if (status == StatusAction::Bridge || status == StatusAction::Crown) {
		bridgeController.formatBridges(*selectedIndexes, &ambList->teeth);
	}

	statusChanged();
}



void ListPresenter::openPatientDialog()
{
	if (patientDialog == NULL) return;

	patientDialog->open(this, *patient);
}

void ListPresenter::openAllergiesDialog()
{
	if (allergiesDialog == NULL) return;

	allergiesDialog->openDialog(this, *patient);

}

void ListPresenter::openProcedureDialog()
{
	auto mList = procedureDialog->openDialog(selectionManager.getSelectedTeethPointers(*selectedIndexes), ambList->teeth, ambList->date);
	

	
	for (Manipulation m : mList)
	{
		qDebug()
			<< static_cast<int>(m.type)
			<< m.code
			<< QString::fromStdString(m.name)
			<< m.tooth
			<< QString::fromStdString(m.diagnosis)
			<< m.price << " .lv";
	}
}

void ListPresenter::statusChanged(std::string status)
{
	ambList->test = status;

	makeEdited();
}

void ListPresenter::setPatient(Patient patient)
{
	*this->patient = patient;
	view->refresh(*this->ambList, *this->patient, *selectedIndexes);
}

void ListPresenter::setAllergies(Allergies allergies)
{
	this->patient->allergies = allergies.allergies;
	this->patient->currentDiseases = allergies.current;
	this->patient->pastDiseases = allergies.past;
	view->refresh(*ambList, *patient, *selectedIndexes);
}
