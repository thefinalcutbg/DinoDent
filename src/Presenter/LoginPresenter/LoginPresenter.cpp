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
    
    if (practiceList.empty())
    {
        ModalDialogBuilder::showError(u8"Докторът не принадлежи към нито една практика");
        return;
    }



    User user;
    
    Doctor& doctorUser = user;
    Practice& practiceUser = user;

    practiceUser = db.getPractice(practiceList[0].rzi);
    doctorUser = doctor.value();

    qDebug() << "рзи на практиката:" << QString{practiceUser.rziCode.c_str()};

    UserManager::setCurrentUser(user);

    loginSuccessful = true;

    if(view)
        view->closeLogin();

}
