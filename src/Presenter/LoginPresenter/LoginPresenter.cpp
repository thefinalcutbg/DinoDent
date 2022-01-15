#include "LoginPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbLogin.h"

#include "Model/User/UserManager.h"

bool LoginPresenter::successful()
{
    UserManager::resetUser();
    practiceList = db.practiceList();

    ModalDialogBuilder::openDialog(this);
    
    //okPressed("220008771", "198312", 0); //fast login

    return loginSuccessful;
}

void LoginPresenter::setView(ILoginView* view)
{
    this->view = view;

    std::vector<std::string> practiceNames;

    for (auto& p : practiceList)
        practiceNames.push_back(p.practice_name);

    view->setPracticeNames(practiceNames);
       
}


void LoginPresenter::okPressed(const std::string& lpk, const std::string& pass, int practiceIdx)
{
    auto result = db.getDoctor(lpk, pass, practiceList[practiceIdx].rziCode);
    
    if (!result.has_value())
    {
        ModalDialogBuilder::showError("Грешно потребителско име или парола");
        return;
    }

    User user;

    Doctor& doctor = user;
    Practice& practice = user;
    
    practice = practiceList[practiceIdx];
    doctor = result.value();

    UserManager::setCurrentUser(user);

    loginSuccessful = true;

    if(view)
        view->closeLogin();

}
