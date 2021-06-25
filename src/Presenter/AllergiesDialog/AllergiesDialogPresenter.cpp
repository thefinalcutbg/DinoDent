#include "AllergiesDialogPresenter.h"
#include "View/ModalDialogBuilder.h"


AllergiesDialogPresenter::AllergiesDialogPresenter(const Patient& patient)
    : patient(&patient), view(nullptr)
{

}

void AllergiesDialogPresenter::setView(IAllergiesDialog* view)
{
    this->view = view;
    view->setData(Allergies{ patient->allergies, patient->currentDiseases, patient->pastDiseases });
}

void AllergiesDialogPresenter::okClicked()
{
    auto data = view->getData();
    database.updateAllergies(patient->id, data.allergies, data.current, data.past);
    allergies = data;
    view->close();
}

std::optional<Allergies> AllergiesDialogPresenter::openDialog()
{
    if(patient)
        ModalDialogBuilder::openDialog(this);

    return allergies;
}
