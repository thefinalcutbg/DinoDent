#include "LoginPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbDoctor.h"
#include "Database/DbPractice.h"
#include "Database/Database.h"
#include "Model/User.h"
#include "Presenter/PracticeManagerPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"

LoginPresenter::LoginPresenter() : m_users(DbDoctor::getDoctorList())
{
 
}

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

    std::vector<std::string> names;
    names.reserve(m_users.size());

    for (auto& cred : m_users) {
        names.push_back(cred.name);
    }

    view->setDoctorList(names);
}

void LoginPresenter::userIdxChanged(int idx)
{
    m_currentIndex = idx;

    if (idx > -1) {
        view->disablePasswordField(m_users[idx].pass.empty());
    }
}


void LoginPresenter::okPressed(const std::string& pass, bool remember)
{
    if (m_currentIndex == -1) return; //maybe add default user?

    if (m_users[m_currentIndex].pass != pass) {
        ModalDialogBuilder::showError("Грешно потребителско име или парола");
        return;
    }

    DbDoctor::setAutoLogin(m_users[m_currentIndex].lpk, remember);
  

    login(m_users[m_currentIndex].lpk);

}

void LoginPresenter::login(const std::string& lpk)
{
    auto doctor = DbDoctor::getDoctor(lpk);

    auto practiceList = DbPractice::getPracticeList(doctor->LPK);

    int practiceIdx;

    switch (practiceList.size())
    {
    case 0:
        ModalDialogBuilder::showError("Лекарят не принадлежи към нито една практика");
        return;

    case 1:
        practiceIdx = 0;
        break;

    default:
        std::vector<std::string> practiceNames;

        for (auto& p : practiceList) {
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

    loginSuccessful = true;

    if (view)
        view->closeLogin();
}
