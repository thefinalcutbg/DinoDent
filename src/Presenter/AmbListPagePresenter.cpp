#include "AmbListPagePresenter.h"
#include "Model/Manipulation/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
AmbListPagePresenter::AmbListPagePresenter(
                                            IAmbListPage* AmbListPage,
                                            ListPresenter* listPresenter) :
    view(AmbListPage),
    listPresenter(listPresenter),
    currentVecPos(-1)
{
    _tabPresenter.setView(AmbListPage);
    _tabPresenter.setListPresenter(listPresenter);
}


void AmbListPagePresenter::newPressed()
{
    PatientDialogPresenter p{};

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.newList(patient.value());
}

bool AmbListPagePresenter::save()
{
    auto currentListInstance = _tabPresenter.currentList();
    if (currentListInstance == nullptr) return true;

    if (currentListInstance->isNew()) {
        return saveAs();
    }

    if (currentListInstance->edited) {
        database.updateAmbList(currentListInstance->amb_list);
    }

    currentListInstance->edited = false;

    view->changeTabName(currentListInstance->getTabName());

    return true;
}

bool AmbListPagePresenter::saveAs()
{
    auto currentListInstance = _tabPresenter.currentList();
    if (currentListInstance == nullptr) return true;

    AmbList& list = currentListInstance->amb_list;

    int newNumber = 0;

    auto map = database.getExistingNumbers(list.date.year);

    if (!list.number) {
        newNumber = database.getNewNumber(list.date.year);
    }
    else {
        newNumber = list.number;
        map[newNumber] = false;
    }

    newNumber = view->openSaveAsDialog(newNumber, map);

    if (!newNumber) return false;

    list.number = newNumber;

    if (currentListInstance->isNew()) {
        database.insertAmbList(list, currentListInstance->patient->id);
    }
    else {
        database.updateAmbList(list);
    }

    currentListInstance->edited = false;

    view->changeTabName(currentListInstance->getTabName());

    return true;
}


bool AmbListPagePresenter::closeTab()
{
    auto current = _tabPresenter.currentList();
    //no need to know which tab. Close button always focuses the tab first.

    if (current->amb_list.isNew() || current->isEdited())
    {
        DialogAnswer answer = view->openSaveDialog(current->getTabName());

        switch (answer)
        {
            case DialogAnswer::Yes:
                if (save()) //if the save is not interrupted
                    break;
                else
                    return false;

            case DialogAnswer::No: break;

            case DialogAnswer::Cancel: return false;
        }
    }

    _tabPresenter.closeList();

    return true;
}

