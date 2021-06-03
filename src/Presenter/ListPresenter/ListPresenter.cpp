#include "ListPresenter.h"
#include "Model/Manipulation/MasterNZOK.h"
#include "Model/User/User.h"

ListPresenter::ListPresenter() :
    view(nullptr),
    ambList(nullptr),
    patient(nullptr),
    patientDialog(nullptr),
    allergiesDialog(nullptr)
{
}

void ListPresenter::setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter)
{
    allergiesDialog = allergiesPresenter;

}

void ListPresenter::setPatientDialog(PatientDialogPresenter* patientDialogPresenter)
{
    patientDialog = patientDialogPresenter;
}


void ListPresenter::setData(ListInstance* inst)
{

    this->ambList = &inst->amb_list;
    this->patient = &inst->patient;
   
    status_presenter.setData(inst->amb_list.teeth, inst->selectedIndexes);
    procedure_presenter.setData(inst->amb_list, inst->patient, inst->selectedIndexes);

    view->refresh
    (
        *this->ambList,
        *this->patient
    );


}

void ListPresenter::setView(IListView* view)
{
    this->view = view;
    status_presenter.setView(view);
    procedure_presenter.setView(view);
}


void ListPresenter::attachEditObserver(EditObserver* observer)
{
    Editor::attachEditObserver(observer);
    status_presenter.attachEditObserver(observer);
    procedure_presenter.attachEditObserver(observer);
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


void ListPresenter::setPatient(Patient patient)
{
    *this->patient = patient;
    view->refresh
    (
        *this->ambList,
        *this->patient
    );
}

void ListPresenter::setAllergies(Allergies allergies)
{
    this->patient->allergies = allergies.allergies;
    this->patient->currentDiseases = allergies.current;
    this->patient->pastDiseases = allergies.past;

    view->refresh
    (
        *this->ambList,
        *this->patient
    );
}
