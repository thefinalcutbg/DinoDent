#include "MainPresenter.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Model/xml.h"
#include "GlobalSettings.h"

#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Database/DbUpdateStatus.h"
#include "Database/DbNotification.h"

#include "Network/HIS/HisToken.h"
#include "Network/Telemetry.h"
#include "Network/GetHSM.h"
#include "Network/IRC/IRCInterface.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/MonthNotifPresenter.h"
#include "Presenter/ReportFilesPresenter.h"
#include "Presenter/LoginPresenter.h"
#include "Presenter/StatisticDialogPresenter.h"
#include "Presenter/RecipientPresenter.h"
#include "Presenter/NewDocPresenter.h"
#include "Presenter/CalendarPresenter.h"

#include "View/Widgets/SplashScreen.h"
#include "View/Widgets/NotificationListDialog.h"
#include "View/Widgets/DinoDent.h"
#include "View/Widgets/SettingsDialog.h"

#include "Printer/SignatureTablet.h"
#include "Printer/FilePaths.h"

MainPresenter MainPresenter::s_singleton;

void MainPresenter::setView(DinoDent* view)
{
    this->view = view;

    auto glob_set = GlobalSettings::getSettings();

    PKCS11::setDriverPaths(glob_set.pkcs11_list);

    User::signatureTablet() = SignatureTablet(glob_set.signer_model, glob_set.signer_filepath);
    FilePaths::setSettings(glob_set.pdfDir, glob_set.subdirStructure);

    firstTimeLogic();

    LoginPresenter login;
    
    auto autologinLPK = DbDoctor::getDoctorAutoLoginLPK();
    
    if (autologinLPK.size()) {
        login.login(autologinLPK);
        view->m_loggedIn = true;
    }
    else
    {
        view->m_loggedIn = login.successful();
    }

    if (!view->m_loggedIn) return;
    
    view->setNotificationIcon(DbNotification::hasNotifications());

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
  
    IRCInterface::connectAs(User::doctor().fname, User::doctor().lname);

}

void MainPresenter::printPressed()
{
    auto tab = TabPresenter::get().currentTab();

    if (tab != nullptr) {
        tab->print();
    }
}

void MainPresenter::newAmbPressed()
{

    NewDocPresenter p("Нов амбулаторен лист");

    auto patient = p.open();

    if (patient.has_value())
        TabPresenter::get().openList(patient.value());
}

void MainPresenter::newPerioPressed()
{
    NewDocPresenter p("Ново пародонтално измерване");

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

void MainPresenter::notificationPressed()
{
    NotificationListDialog d;
    d.exec();

    view->setNotificationIcon(DbNotification::hasNotifications());
}

void MainPresenter::pdfPressed()
{
    auto tab = TabPresenter::get().currentTab();

    if (tab){
        tab->pdfPrint();
    }
}

void MainPresenter::newPrescriptionPressed()
{
    NewDocPresenter p("Нова рецепта");

    auto patient = p.open();

    if (patient.has_value())
        TabPresenter::get().openPerscription(patient.value());
}

void MainPresenter::openCalendar()
{
    TabPresenter::get().openCalendar();
}

void MainPresenter::showBrowser()
{
    ModalDialogBuilder::openBrowserDialog();
}


void MainPresenter::pisDialog()
{
    if (!User::hasNhifContract()) {
        ModalDialogBuilder::showMessage("За да достъпите тази функционалност, попълнете данните на договора с НЗОК от настройки!");
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
    ModalDialogBuilder::openSettingsDialog(static_cast<int>(SettingsDialog::Tab::General));

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );

    IRCInterface::connectAs(User::doctor().fname, User::doctor().lname);

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
    IRCInterface::disconnect();

    PKCS11::cleanup();

    HisToken::nullifyToken();

    ReportFilesPresenter::clearCache();
    MonthNotifPresenter::clearCache();

    DbDoctor::setAutoLogin(User::doctor().LPK, false);

    view->setNotificationIcon(0);

    LoginPresenter login;

    if (login.successful() == false)
    {
        view->exitProgram();
    }

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
        );

    view->setNotificationIcon(DbNotification::hasNotifications());

    IRCInterface::connectAs(User::doctor().fname, User::doctor().lname);

    Telemetry::sendData();

}

void MainPresenter::userSettingsPressed()
{
    ModalDialogBuilder::openSettingsDialog(static_cast<int>(SettingsDialog::Tab::Doctor));

    view->setUserLabel(
        User::doctor().getFullName(),
        User::practice().name
    );

    if (!User::isValid()) return;

    IRCInterface::connectAs(User::doctor().fname, User::doctor().lname);
}

bool MainPresenter::closeAllTabs()
{
    return TabPresenter::get().permissionToLogOut();
}

void MainPresenter::firstTimeLogic()
{
    if(!DbPractice::noPractices()) return;
  
    Practice p;
    p.name = "DinoDent";
    p.practice_address = 68134;
    p.firm_address = "гр. София";
    p.legal_entity = 2;
    p.rziCode = "0000000000";
    p.bulstat = "000000000";
            
    Doctor d;
    d.fname = "Иван";
    d.mname = "Иванов";
    d.lname = "Иванов";
    d.LPK = "000000000";
    d.hisSpecialty = 0;
    d.phone = "";
    
    PracticeDoctor pd;
    pd.lpk = d.LPK;
    pd.admin = true;
    pd.specialty = NhifSpecialty::None;

    DbDoctor::insertDoctor(d);
    DbDoctor::setAutoLogin(d.LPK, true);
    DbPractice::insertPractice(p);
    DbPractice::setDoctorsPracticeList({ pd }, p.rziCode);
   
    ModalDialogBuilder::showMessage(
        "Стартирате програмата за първи път. "
        "Създаден е примерен профил. "
        "Можете да редактирате данните от \"Настройки\" "
    );
}
