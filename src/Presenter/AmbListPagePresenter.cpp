#include "AmbListPagePresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
AmbListPagePresenter::AmbListPagePresenter() :
    view(nullptr)
{}

void AmbListPagePresenter::setView(IAmbListPage* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
}

void AmbListPagePresenter::newPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.newList(patient.value());
}

bool AmbListPagePresenter::save()
{
    auto list = _tabPresenter.currentList();

    if (list == nullptr) return true;

    if (list->isNew()) {
        return saveAs();
    }

    if (list->edited) {
        database.updateAmbList(list->amb_list);
    }

    list->edited = false;

    view->tabView()->changeTabName(list->getTabName());

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

    view->tabView()->changeTabName(currentListInstance->getTabName());

    return true;
}


bool AmbListPagePresenter::closeTab()
{
    auto list = _tabPresenter.currentList();
    //no need to know which tab. Close button always focuses the tab first.

    if (list->amb_list.isNew() || list->isEdited())
    {
        DialogAnswer answer = view->openSaveDialog(list->getTabName());

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

    _tabPresenter.removeCurrentList();

    return true;
}

bool AmbListPagePresenter::closeAllTabs()
{
    while (_tabPresenter.currentList())
    {
       bool canceled = !closeTab();
       if (canceled) return false;
    }
    
    return true;
}

