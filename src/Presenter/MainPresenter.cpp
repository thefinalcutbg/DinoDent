#include "MainPresenter.h"
#include "Network/HIS/HisToken.h"
#include "Model/Dental/NhifProcedures.h"
#include "Presenter/PatientDialogPresenter.h"
#include "View/Printer.h"
#include "Presenter/LoginPresenter.h"
#include "Presenter/DoctorDialogPresenter.h"
#include "Network/PKCS11.h"
#include "Model/User.h"
#include "Model/xml.h"
#include "Presenter/PracticeDialogPresenter.h"
#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Database/DbUpdateStatus.h"
#include "View/Printer.h"
#include <qdebug.h>
MainPresenter::MainPresenter()
{}

void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    m_tabPresenter.setView(view->tabView());
    m_browserPresenter.setTabPresenter(&m_tabPresenter);

    if (DbPractice::noPractices())
    {
        ModalDialogBuilder::showMessage("Стартирате програмата за първи път. Моля добавете практика.");

        PracticeDialogPresenter p{};
        auto result = p.open();

        if (result.has_value())
        {
            DbPractice::insertPractice(result.value().practice);
            DbPractice::setDoctorsPracticeList(result.value().doctorsList, result.value().practice.rziCode);
        }
        /*
        else
        {
            view->m_loggedIn = false;
            return;
        }
        */
    }

    LoginPresenter login;

    auto auto_login = DbDoctor::getLpkAndPassAutoLogin();

    if (auto_login.first.size()) {
        login.okPressed(auto_login.first, auto_login.second, true);
        view->m_loggedIn = true;
    }
    else
    {
        view->m_loggedIn = login.successful();
    }

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );

    if (view->m_loggedIn)
    {
        //medications update
        if (DbUpdateStatus::lastUpdated(DynamicNum::Medication).isFromPreviousMonths(Date::currentDate()))
        {
            med_update_service.update();
        }
    }

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

void MainPresenter::newPerscriptionPressed()
{
    PatientDialogPresenter p;

    auto patient = p.open();

    if (patient.has_value())
        m_tabPresenter.openPerscription(patient.value());
}

void MainPresenter::showListSelector()
{
    m_browserPresenter.openDialog();
}


void MainPresenter::pisDialog()
{

    ModalDialogBuilder::pisDialog(&m_notifPresenter);

}

void MainPresenter::statisticPressed()
{
    m_tabPresenter.openStatistic();
}

void MainPresenter::settingsPressed()
{
    if (!User::isAdmin())
    {
        ModalDialogBuilder::showError("Нямате администраторски права, за да влезете в настройки");
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

    HisToken::nullifyToken();

    DbDoctor::setAutoLogin(User::doctor().LPK, false);

    

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

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );
}

bool MainPresenter::closeAllTabs()
{
    return m_tabPresenter.permissionToLogOut();
}

