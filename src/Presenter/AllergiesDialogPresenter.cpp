#include "AllergiesDialogPresenter.h"
#include "View/ModalDialogBuilder.h"


AllergiesDialogPresenter::AllergiesDialogPresenter(const Patient& patient)
    : view(nullptr), allergies{ Allergies{ patient.allergies, patient.currentDiseases, patient.pastDiseases } }
{

}

void AllergiesDialogPresenter::setView(IAllergiesDialog* view)
{
    this->view = view;
    view->setData(allergies.value());
    allergies.reset();
}

void AllergiesDialogPresenter::okClicked()
{
    auto data = view->getData();

    allergies = data;
    view->close();
}

std::optional<Allergies> AllergiesDialogPresenter::openDialog()
{
    ModalDialogBuilder::openDialog(this);

    return allergies;
}
