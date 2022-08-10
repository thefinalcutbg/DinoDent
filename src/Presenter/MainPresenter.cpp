#include "MainPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "View/Printer/Printer.h"
#include "Presenter/LoginPresenter/LoginPresenter.h"
#include "Presenter/DoctorDialogPresenter/DoctorDialogPresenter.h"
#include "Network/Common/PKCS11.h"
#include "Model/User/User.h"
#include "Model/XML/xml.h"
#include "Presenter/AddPracticePresenter/AddPracticePresenter.h"
#include "Database/DbPractice.h"
#include "View/Printer/Printer.h"

MainPresenter::MainPresenter()
{}

void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    m_tabPresenter.setView(view->tabView());
    m_listSelector.setTabPresenter(&m_tabPresenter);

    if (DbPractice::noPractices())
    {
        ModalDialogBuilder::showMessage("Стартирате програмата за първи път. Моля добавете практика.");

        AddPracticePresenter p;
        auto result = p.open();

        if (result.has_value())
        {
            DbPractice::insertPractice(result.value().practice);
            DbPractice::setDoctorsPracticeList(result.value().doctorsList, result.value().practice.rziCode);
            ModalDialogBuilder::showMessage("Зареден е ценоразпис с нулеви цени. Можете да го редактирате от Насторйки");
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
    auto tab = m_tabPresenter.currentTab();

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
        m_tabPresenter.openList(patient.value());
}

void MainPresenter::newPerioPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        m_tabPresenter.openPerio(patient.value());
}

void MainPresenter::showListSelector()
{
    m_listSelector.openDialog();
}


void MainPresenter::pisDialog()
{
    if (!User::practice().nzok_contract)
    {
        ModalDialogBuilder::showError(
            u8"Моля попълнете данните на договора с НЗОК от настройки"
        );
        return;
    }

    ModalDialogBuilder::pisDialog(&m_notifPresenter);

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
    if(m_tabPresenter.currentTab())
        return m_tabPresenter.currentTab()->save();

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
    return m_tabPresenter.permissionToLogOut();
}

