#include "MainPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/Printer/Printer.h"
#include "Presenter/LoginPresenter/LoginPresenter.h"
#include "Presenter/DoctorDialogPresenter/DoctorDialogPresenter.h"
#include "Presenter/InvoicePresenter/InvoicePresenter.h"
#include "Model/User/UserManager.h"
#include "Model/XML/xml.h"


MainPresenter::MainPresenter() :
    view(nullptr)
{}

void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
    listSelector_.setTabPresenter(&_tabPresenter);

    LoginPresenter login;

    view->m_initialized = login.successful();

    view->setUserLabel(
        UserManager::currentUser().doctor.getFullName(),
        UserManager::currentUser().practice.name
    );

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

void MainPresenter::generateReport()
{
    if (!UserManager::currentUser().practice.nzok_contract.has_value())
    {
        ModalDialogBuilder::showError
        (u8"За да генерирате отчет, моля попълнете данните на договора с НЗОК от настройки");
        return;
    }

    std::optional<ReportDialogResult> result;

    ModalDialogBuilder::openDialog(result);

    if (result.has_value()) {

        auto err = XML::saveXMLreport(result.value().month, result.value().year, result.value().path);

        if (err.has_value())
            ModalDialogBuilder::showErrorList(err.value());
    }
        

   

}

void MainPresenter::generateInvoice()
{
    InvoicePresenter p;
    p.open();
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

void MainPresenter::logOut()
{
    if (!closeAllTabs()) return;

    view->setUserLabel("", "");

    LoginPresenter login;

    if (login.successful() == false)
    {
        view->exitProgram();
    }

    view->setUserLabel(
        UserManager::currentUser().doctor.getFullName(),
        UserManager::currentUser().practice.name
        );
    
}

void MainPresenter::userSettingsPressed()
{
    DoctorDialogPresenter p(UserManager::currentUser().doctor);
    auto doctor = p.open();

    if (doctor.has_value())
        UserManager::setCurrentDoctor(doctor.value());
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

