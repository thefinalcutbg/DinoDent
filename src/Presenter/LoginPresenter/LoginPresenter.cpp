#include "LoginPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbLogin.h"
#include <QDebug>
#include "Model/User/UserManager.h"



bool LoginPresenter::successful()
{
    UserManager::resetUser();

    ModalDialogBuilder::openDialog(this); //for fast login comment this out 
    
   // okPressed("220008771", "198312"); //and remove this as comment

    return loginSuccessful;
}

void LoginPresenter::setView(ILoginView* view)
{
    this->view = view;

 //   std::vector<std::string> practiceNames;

   // for (auto& p : practiceList)
   //     practiceNames.push_back(p.practice_name);

 //   view->setPracticeNames(practiceNames);
       
}


void LoginPresenter::okPressed(const std::string& lpk, const std::string& pass)
{
    auto doctor = db.getDoctor(lpk, pass);

    if (!doctor.has_value())
    {
        ModalDialogBuilder::showError(u8"Грешно потребителско име или парола");
        return;
    }

    auto practiceList = db.getPracticeList(lpk);
    
    int practiceIdx;

    switch (practiceList.size())
    {
    case 0:
        ModalDialogBuilder::showError(u8"Докторът не принадлежи към нито една практика");
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

    User user;

    user.practice = db.getPractice(practiceList[practiceIdx].rzi);
    user.doctor = doctor.value();

    UserManager::setCurrentUser(user);

    loginSuccessful = true;

    if(view)
        view->closeLogin();

}
