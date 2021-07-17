#include "ListPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/User/User.h"
#include "../TabPresenter/ListInstance.h"

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

ListPresenter::ListPresenter() :
    view(nullptr),
    ambList(nullptr)
{
    status_presenter.setProcedurePresenter(&procedure_presenter);
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
    view->setPresenter(this);
    status_presenter.setView(view);
    procedure_presenter.setView(view);
}


void ListPresenter::attachEditObserver(EditObserver* observer)
{
    Editor::attachEditObserver(observer);
    status_presenter.attachEditObserver(observer);
    procedure_presenter.attachEditObserver(observer);
}

void ListPresenter::chargeChanged(int index)
{
    ambList->charge = static_cast<Charge>(index);
    makeEdited();
}


void ListPresenter::openPatientDialog()
{

    PatientDialogPresenter p{ *patient.lock() };
  
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
    AllergiesDialogPresenter p(*patient.lock());

    auto data = p.openDialog();

    if (!data.has_value()) return;

    auto& d = data.value();

    patient.lock()->allergies = d.allergies;
    patient.lock()->currentDiseases = d.current;
    patient.lock()->pastDiseases = d.past;

    view->refresh
    (
        *this->ambList,
        *patient.lock()
    );
}
