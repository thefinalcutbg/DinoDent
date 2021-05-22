#include "AllergiesDialogPresenter.h"

AllergiesDialogPresenter::AllergiesDialogPresenter(IAllergiesDialog* view) :
    view(view), database(database), requestor(nullptr)
{
//    ../Torque/src/Presenter/AllergiesDialog/AllergiesDialogPresenter.cpp:3:1: warning: ‘AllergiesDialogPresenter::database’ is initialized with itself [-Winit-self]
//        3 | AllergiesDialogPresenter::AllergiesDialogPresenter(IAllergiesDialog* view) :
//          | ^~~~~~~~~~~~~~~~~~~~~~~~
}

void AllergiesDialogPresenter::setAllergies(std::string allergies, std::string current, std::string past)
{
    database.updateAllergies(patient_id, allergies, current, past);

    if(requestor != NULL)
    requestor->setAllergies(Allergies(allergies, current, past));
}

void AllergiesDialogPresenter::openDialog(AllergiesDialogRequestor* requestor, Patient patient)
{
    this->requestor = requestor;
    patient_id = patient.id;
    view->open(patient.allergies, patient.currentDiseases, patient.pastDiseases);
}
