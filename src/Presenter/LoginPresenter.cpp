#include "LoginPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbDoctor.h"
#include "Database/DbPractice.h"
#include "Database/Database.h"
#include "Model/User.h"
#include "Presenter/PracticeManagerPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"

bool LoginPresenter::successful()
{
    User::resetUser();

    ModalDialogBuilder::openDialog(*this);

    return loginSuccessful;
}

void LoginPresenter::practiceListPressed()
{
    view->closeLogin();
    PracticeManagerPresenter p;
    ModalDialogBuilder::openDialog(p);
    ModalDialogBuilder::openDialog(*this);
}


void LoginPresenter::setView(ILoginView* view)
{
    this->view = view;
}


void LoginPresenter::okPressed(const std::string& lpk, const std::string& pass, bool remember)
{
    auto doctor = DbDoctor::getDoctor(lpk, pass);

    if (!doctor.has_value())
    {
        ModalDialogBuilder::showError("Грешно потребителско име или парола");
        return;
    }

    auto practiceList = DbPractice::getPracticeList(lpk);
    
    int practiceIdx;

    switch (practiceList.size())
    {
    case 0:
        ModalDialogBuilder::showError("Докторът не принадлежи към нито една практика");
        return;

    case 1:
        practiceIdx = 0; 
        break;

    default:
        std::vector<std::string> practiceNames;

        for (auto& p : practiceList){
            practiceNames.push_back(p.name);
        }

        practiceIdx = view->practiceUserChoice(practiceNames);

        if (practiceIdx == -1) return; //-1 means user has canceled the dialog

        break;
    }

    
    doctor->specialty = static_cast<NhifSpecialty>(std::get<1>(DbDoctor::getAdminAndSpecialty(doctor->LPK, practiceList[practiceIdx].rzi)));
    User::setCurrentPractice(DbPractice::getPractice(practiceList[practiceIdx].rzi));
    User::setCurrentDoctor(doctor.value());
    NetworkManager::setTimeout(User::practice().settings.timeout);

    if (remember)DbDoctor::setAutoLogin(lpk, true);

    loginSuccessful = true;

    if(view)
        view->closeLogin();

}
