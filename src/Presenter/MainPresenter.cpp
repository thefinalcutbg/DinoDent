#include "MainPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/Printer/Printer.h"
#include "Presenter/LoginPresenter/LoginPresenter.h"

MainPresenter::MainPresenter() :
    view(nullptr)
{}

void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
    listSelector_.setTabPresenter(&_tabPresenter);

    LoginPresenter login;

    view->m_initialized = login.tryLogin();
    
}

void MainPresenter::printPressed()
{
    auto tab = _tabPresenter.currentTab();

    if (tab != nullptr)
        tab->print();
}

void MainPresenter::newAmbPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.openList(patient.value());
}

void MainPresenter::newPerioPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        _tabPresenter.openPerio(patient.value());
}

void MainPresenter::showListSelector()
{
    listSelector_.openDialog();
}

bool MainPresenter::save() 
{
    if(_tabPresenter.currentTab())
        return _tabPresenter.currentTab()->save();

    return true;
}

bool MainPresenter::saveAs()
{
    if (_tabPresenter.currentTab())
        return _tabPresenter.currentTab()->saveAs();

    return true;
}


bool MainPresenter::closeTab()
{
    if (!_tabPresenter.currentTab()->close()) return false;

    _tabPresenter.removeCurrentTab();

    return true;
}

bool MainPresenter::closeAllTabs()
{
    while (_tabPresenter.currentTab())
    {
       bool canceled = !closeTab();
       if (canceled) return false;
    }
    
    return true;
}

