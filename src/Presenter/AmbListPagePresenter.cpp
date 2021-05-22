#include "AmbListPagePresenter.h"

AmbListPagePresenter::AmbListPagePresenter(
                                            IAmbListPage* AmbListPage,
                                            PatientDialogPresenter* patientDialog,
                                            ListPresenter* listView) :
    view(AmbListPage),
    patientDialog(patientDialog),
    listView(listView),
    currentVecPos(-1),
    currentListInstance(nullptr)
{
    //listView->attachEditObserver(this); //this is NOT working, I have to set it from the AmbListView
}


void AmbListPagePresenter::newPressed()
{
    patientDialog->open(this);


}

void AmbListPagePresenter::setPatient(Patient patient)
{

    AmbList* ambList = database.getList(patient.id, Date::currentMonth(), Date::currentYear());

    for (int i = 0; i < list_instance.size(); i++)
    {
        if (list_instance[i].amb_list.id == ambList->id && patient.id == list_instance[i].patient.id)
        {
            view->focusTab(i);
            return;
        }
    }

    list_instance.emplace_back(*ambList, patient);

    delete ambList;

    view->newTab(list_instance.size() - 1, list_instance.back().getTabName());
}

void AmbListPagePresenter::tabChanged(int vecPos)
{
    if (vecPos == -1)
    {
        currentListInstance = NULL;
        currentVecPos = -1;
        return;
    }
    currentListInstance = &list_instance[vecPos];
    currentVecPos = vecPos;

    listView->setData(currentListInstance);
}

void AmbListPagePresenter::notify()
{
    view->changeTabName(currentListInstance->getTabName());
}

bool AmbListPagePresenter::save()
{
    if (currentListInstance == NULL) return true;

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
    if (currentListInstance == NULL) return true;

    AmbList& list = currentListInstance->amb_list;

    int newNumber = 0;

    //what happens if the list is from a previous year?
    //Then current year will mess things up

    auto map = database.getExistingNumbers(Date::currentYear());

    if (!list.number) {
        newNumber = database.getNewNumber(Date::currentYear());
    }
    else {
        newNumber = list.number;
        map[newNumber] = false;
    }

    newNumber = view->openSaveAsDialog(newNumber, map);

    if (!newNumber) return false;

    list.number = newNumber;

    if (currentListInstance->isNew()) {
        database.insertAmbList(list, currentListInstance->patient.id);
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

    //no need to know which tab. Close button always focuses the tab first.
    if (currentListInstance->amb_list.isNew() || currentListInstance->isEdited())
    {
        DialogAnswer answer = view->openSaveDialog(currentListInstance->getTabName());

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

    list_instance.erase(list_instance.begin() + currentVecPos);
    view->removeCurrentTab();

    return true;
}

