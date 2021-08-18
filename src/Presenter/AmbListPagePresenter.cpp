#include "AmbListPagePresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/ErrorMessage.h"
#include "View/Printer/Printer.h"

bool AmbListPagePresenter::listIsValid()
{
    ListInstance* list = _tabPresenter.currentList();

    if (list == nullptr) return false;

    AmbListValidator checker(*list);

    if (checker.ambListIsValid())
        return true;

    showError(checker.getErrorMsg());

    return false;
}

AmbListPagePresenter::AmbListPagePresenter() :
    view(nullptr)
{}

void AmbListPagePresenter::setView(IAmbListPage* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
    listSelector_.setTabPresenter(&_tabPresenter);
}

void AmbListPagePresenter::printPressed()
{
    auto list = _tabPresenter.currentList();

    if (!list) 
    {
        Print::ambList();
        return;
    }

   Print::ambList(list->amb_list, *list->patient.get());
}

void AmbListPagePresenter::newPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.openList(patient.value());
}

void AmbListPagePresenter::showListSelector()
{
    listSelector_.openDialog();
}

bool AmbListPagePresenter::save()
{
    auto list = _tabPresenter.currentList();

    if (list == nullptr) return true;

    if (list->isNew()) {
        return saveAs();
    }

    if (list->edited) {
        if (!listIsValid()) return false;
        amb_db.updateAmbList(list->amb_list);
        listSelector_.refreshModel();
    }

    list->edited = false;
    listSelector_.refreshModel();
    view->tabView()->changeTabName(list->getTabName());



    return true;
}
#include <QDebug>
bool AmbListPagePresenter::saveAs()
{
    auto currentListInstance = _tabPresenter.currentList();

    if (currentListInstance == nullptr) return true;

    if (!listIsValid()) return false;

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

