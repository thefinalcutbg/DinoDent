#include "AmbListPagePresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
AmbListPagePresenter::AmbListPagePresenter() :
    view(nullptr)
{}

void AmbListPagePresenter::setView(IAmbListPage* view)
{
    this->view = view;

    tabPresenter_.setView(view->tabView());
    listSelector_.setTabPresenter(&tabPresenter_);
}

void AmbListPagePresenter::newPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        tabPresenter_.openList(patient.value());
}

void AmbListPagePresenter::showListSelector()
{
    listSelector_.openDialog();
}

bool AmbListPagePresenter::save()
{
    auto list = tabPresenter_.currentList();

    if (list == nullptr) return true;

    if (list->isNew()) {
        return saveAs();
    }

    if (list->edited) {
        amb_db.updateAmbList(list->amb_list);
        listSelector_.refreshModel();
    }

    list->edited = false;
    listSelector_.refreshModel();
    view->tabView()->changeTabName(list->getTabName());



    return true;
}

bool AmbListPagePresenter::saveAs()
{
    auto currentListInstance = tabPresenter_.currentList();

    if (currentListInstance == nullptr) return true;

    AmbList& list = currentListInstance->amb_list;

    int newNumber = 0;

    auto map = amb_db.getExistingNumbers(list.date.year);

    if (!list.number) {
        newNumber = amb_db.getNewNumber(list.date.year);
    }
    else {
        newNumber = list.number;
        map[newNumber] = false;
    }

    newNumber = view->openSaveAsDialog(newNumber, map);

    if (!newNumber) return false;

    list.number = newNumber;

    if (currentListInstance->isNew()) {
        amb_db.insertAmbList(list, currentListInstance->patient->id);
    }
    else {
        amb_db.updateAmbList(list);
    }

    listSelector_.refreshModel();

    currentListInstance->edited = false;

    view->tabView()->changeTabName(currentListInstance->getTabName());

    return true;
}


bool AmbListPagePresenter::closeTab()
{
    auto list = tabPresenter_.currentList();
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

    tabPresenter_.removeCurrentList();

    return true;
}

bool AmbListPagePresenter::closeAllTabs()
{
    while (tabPresenter_.currentList())
    {
       bool canceled = !closeTab();
       if (canceled) return false;
    }
    
    return true;
}

