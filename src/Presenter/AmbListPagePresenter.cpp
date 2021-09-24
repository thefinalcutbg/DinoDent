#include "AmbListPagePresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/ErrorMessage.h"
#include "View/Printer/Printer.h"


AmbListPagePresenter::AmbListPagePresenter() :
    view(nullptr)
{}

void AmbListPagePresenter::setView(IAmbListPage* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
    listSelector_.setTabPresenter(&_tabPresenter);
    
}
#
void AmbListPagePresenter::printPressed()
{

    auto tab = _tabPresenter.currentTab();

    if (tab != nullptr)
    {   
        qDebug() << "printing?";
        tab->print();
    }
}

void AmbListPagePresenter::newAmbPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.openList(patient.value());
}

void AmbListPagePresenter::newPerioPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.openPerio(patient.value());
}

void AmbListPagePresenter::showListSelector()
{
    listSelector_.openDialog();
}

bool AmbListPagePresenter::save() 
{
    if(_tabPresenter.currentTab())
        return _tabPresenter.currentTab()->save();

    return true;
}

bool AmbListPagePresenter::saveAs()
{
    if (_tabPresenter.currentTab())
        return _tabPresenter.currentTab()->saveAs();

    return true;
}


bool AmbListPagePresenter::closeTab()
{
    if (!_tabPresenter.currentTab()->close()) return false;

    _tabPresenter.removeCurrentTab();

    return true;
}

bool AmbListPagePresenter::closeAllTabs()
{
    while (_tabPresenter.currentTab())
    {
       bool canceled = !closeTab();
       if (canceled) return false;
    }
    
    return true;
}

