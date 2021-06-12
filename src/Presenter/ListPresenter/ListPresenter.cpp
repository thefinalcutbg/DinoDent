#include "ListPresenter.h"
#include "Model/Manipulation/MasterNZOK.h"
#include "Model/User/User.h"
#include "../TabPresenter/ListInstance.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
ListPresenter::ListPresenter() :
    view(nullptr),
    ambList(nullptr),
    allergiesDialog(nullptr)
{
}

void ListPresenter::setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter)
{
    allergiesDialog = allergiesPresenter;

}


void ListPresenter::setData(ListInstance* inst)
{

    this->ambList = &inst->amb_list;
    this->patient = inst->patient;
   
    status_presenter.setData(inst->amb_list.teeth, inst->selectedTeeth);
    procedure_presenter.setData(inst->amb_list, *patient.lock(), inst->selectedTeeth);

    view->refresh
    (
        *this->ambList,
        *patient.lock()
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
    PatientDialogPresenter p{ *this->patient.lock() };
    auto patient = p.open();

    if (!patient.has_value()) return;

    *this->patient.lock() = patient.value();
    view->refresh
    (
        *this->ambList,
        *this->patient.lock()
    );

}

void ListPresenter::openAllergiesDialog()
{
    if (allergiesDialog == NULL) return;

    allergiesDialog->openDialog(this, *patient.lock());
   
}

void ListPresenter::setAllergies(Allergies allergies)
{
    patient.lock()->allergies = allergies.allergies;
    patient.lock()->currentDiseases = allergies.current;
    patient.lock()->pastDiseases = allergies.past;

    view->refresh
    (
        *this->ambList,
        *patient.lock()
    );
}
