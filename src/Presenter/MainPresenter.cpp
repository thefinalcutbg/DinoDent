#include "MainPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/Printer/Printer.h"
#include "Presenter/LoginPresenter/LoginPresenter.h"
#include "Presenter/DoctorDialogPresenter/DoctorDialogPresenter.h"
#include "Network/PKCS11.h"
#include "Model/User/User.h"
#include "Model/XML/xml.h"
#include "Presenter/AddPracticePresenter/AddPracticePresenter.h"
#include "Presenter/ReportDialogPresenter/ReportDialogPresenter.h"
#include "Database/DbPractice.h"
#include "View/Printer/Printer.h"

MainPresenter::MainPresenter() :
    view(nullptr),
    m_notifLoader(&_tabPresenter)
{}

void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    _tabPresenter.setView(view->tabView());
    listSelector_.setTabPresenter(&_tabPresenter);

    if (DbPractice::noPractices())
    {
        ModalDialogBuilder::showMessage("Стартирате програмата за първи път. Моля добавете практика.");

        AddPracticePresenter p;
        auto result = p.open();

        if (result.has_value())
        {
            DbPractice::insertPractice(result.value().practice);
            DbPractice::setDoctorsPracticeList(result.value().doctorsList, result.value().practice.rziCode);
        }
        else
        {
            view->m_initialized = false;
            return;
        }
    }

    LoginPresenter login;

    view->m_initialized = login.successful();

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );

}

void MainPresenter::printPressed()
{
    auto tab = _tabPresenter.currentTab();

    if (tab != nullptr) {
        tab->print();
     }
    else {
        Print::ambList();
    }
 
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
    if (!User::practice().nzok_contract.has_value())
    {
        ModalDialogBuilder::showError(
            u8"За да генерирате отчет, моля попълнете данните на договора с НЗОК от настройки"
        );
        return;
    }

    ReportDialogPresenter p;

    ModalDialogBuilder::openDialog(&p);
       
}

void MainPresenter::generateInvoice()
{

    if (!User::practice().nzok_contract)
    {
        ModalDialogBuilder::showError(
            u8"За да заредите месечно известие, моля попълнете данните на договора с НЗОК от настройки"
        );
        return;
    }

    m_notifLoader.loadNotification();

}

void MainPresenter::settingsPressed()
{
    if (!User::isAdmin())
    {
        ModalDialogBuilder::showError(u8"Нямате администраторски права, за да влезете в настройки");
        return;
    }

    ModalDialogBuilder::openSettingsDialog();

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );
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

    PKCS11::unloadModule();

    LoginPresenter login;

    if (login.successful() == false)
    {
        view->exitProgram();
    }

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
        );
    
}

void MainPresenter::userSettingsPressed()
{
    DoctorDialogPresenter p(User::doctor());
    auto doctor = p.open();

    if (doctor.has_value())
        User::setCurrentDoctor(doctor.value());
}

bool MainPresenter::closeAllTabs()
{
    return _tabPresenter.permissionToLogOut();
}

