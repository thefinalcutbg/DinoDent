#include "MainPresenter.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Model/xml.h"

#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Database/DbUpdateStatus.h"

#include "Network/HIS/HisToken.h"
#include "Network/Telemetry.h"
#include "Network/PKCS11.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/MonthNotifPresenter.h"
#include "Presenter/PracticeDialogPresenter.h"
#include "Presenter/ReportFilesPresenter.h"
#include "Presenter/MonthNotifPresenter.h"
#include "Presenter/LoginPresenter.h"
#include "Presenter/DoctorDialogPresenter.h"
#include "Presenter/StatisticDialogPresenter.h"
#include "Presenter/RecipientPresenter.h"

#include "View/Printer.h"
#include "View/Widgets/SplashScreen.h"

MainPresenter MainPresenter::s_singleton;



void MainPresenter::setView(IMainView* view)
{
    this->view = view;

    if (!firstTimeLogic()) { return; }

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

    if (!view->m_loggedIn) return;
    
    Telemetry::sendData();

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );


    //medications update

    auto lasMedUpdate = DbUpdateStatus::lastUpdated(DynamicNum::Medication);
    auto currentDate = Date::currentDate();

    if(lasMedUpdate.isFromPreviousMonths(currentDate) ||
        lasMedUpdate.day < currentDate.day-5
       )
    {
        med_update_service.update();
    }
  

}


void MainPresenter::printPressed()
{
    auto tab = TabPresenter::get().currentTab();

    if (tab != nullptr) {
        tab->print();
     }
    else {
        Print::ambList();
    }
 
}

void MainPresenter::newAmbPressed()
{
    PatientDialogPresenter p("Нов амбулаторен лист");

    auto patient = p.open();

    if (patient.has_value())
        TabPresenter::get().openList(patient.value());
}

void MainPresenter::newPerioPressed()
{
    PatientDialogPresenter p("Ново пародонтално измерване");

    auto patient = p.open();

    if (patient.has_value())
        TabPresenter::get().openPerio(patient.value());
}

void MainPresenter::newInvoicePressed()
{
    RecipientPresenter p;

    auto result = p.openDialog();

    if (!result.has_value()) {
        return;
    }

    TabPresenter::get().openInvoice(result.value());
}

void MainPresenter::newPrescriptionPressed()
{
    PatientDialogPresenter p("Нова рецепта");

    auto patient = p.open();

    if (patient.has_value())
        TabPresenter::get().openPerscription(patient.value());
}

void MainPresenter::showBrowser()
{
    ModalDialogBuilder::openBrowserDialog();
}


void MainPresenter::pisDialog()
{
    if (!User::hasNhifContract()) {
        ModalDialogBuilder::showMessage("Моля, попълнете данните на договора с НЗОК от настройки!");
        return;
    }

    MonthNotifPresenter p;
    ModalDialogBuilder::pisDialog(&p);

}

void MainPresenter::statisticPressed()
{
    StatisticDialogPresenter p;
    p.openDialog();
}

void MainPresenter::settingsPressed()
{
    ModalDialogBuilder::openSettingsDialog();

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );

    //refreshing the data to the view (e.g. issuer in invoices)
    auto currentTab = TabPresenter::get().currentTab();
    if (currentTab) {
        currentTab->setCurrent();
    }
    
}

bool MainPresenter::save() 
{
    if(TabPresenter::get().currentTab())
        return TabPresenter::get().currentTab()->save();

    return true;
}

void MainPresenter::logOut()
{
    if (!closeAllTabs()) return;

    view->setUserLabel("", "");

    PKCS11::cleanup();

    HisToken::nullifyToken();

    ReportFilesPresenter::clearCache();
    MonthNotifPresenter::clearCache();

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
    
    Telemetry::sendData();

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
    return TabPresenter::get().permissionToLogOut();
}

bool MainPresenter::firstTimeLogic()
{
    if(!DbPractice::noPractices()) return true;
   
    std::string descr = "Стартирате програмата за първи път. Изберете опция: ";

    std::vector<std::string> options { "Добавяне на практика", "Просто искам да тествам програмата" };

    auto result = ModalDialogBuilder::openButtonDialog(options, "DinoDent", descr);

    switch (result)
    {
        case 0:
        {
            PracticeDialogPresenter p{};
            auto result = p.open();

            if (!result) { return false; }
           
            DbPractice::insertPractice(result.value().practice);
            DbPractice::setDoctorsPracticeList(result.value().doctorsList, result.value().practice.rziCode);
           
            return true;
        }
        case 1:
        {
            Practice p;
            p.name = "DinoDent - тестови режим";
            p.practice_address = 68134;
            p.firm_address = " ";
            p.legal_entity = 2;
            p.rziCode = "2200000000";
            p.bulstat = "000000000";
            
            Doctor d;
            d.fname = "Иван";
            d.mname = "Иванов";
            d.lname = "Иванов";
            d.LPK = "000000000";
            d.hisSpecialty = 2081;
            d.phone = "1234567890";
            

            PracticeDoctor pd;
            pd.lpk = d.LPK;
            pd.admin = true;

            DbDoctor::insertDoctor(d);
            DbDoctor::setAutoLogin(d.LPK, true);
            DbPractice::insertPractice(p);
            DbPractice::setDoctorsPracticeList({ pd }, p.rziCode);

            return true;
        }
        default: return false;
    }
}