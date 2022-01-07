#include "LoginPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbLogin.h"


bool LoginPresenter::tryLogin()
{
    ModalDialogBuilder::openDialog(this);

    return loginSuccessful;
}

void LoginPresenter::setView(ILoginView* view)
{
    this->view = view;

    practiceList = db.practiceList();

    std::vector<std::string> practiceNames;

    for (auto& p : practiceList)
        practiceNames.push_back(p.name);

    view->setPracticeNames(practiceNames);
    
}

#include "Model/User/UserManager.h"

void LoginPresenter::okPressed(const std::string& lpk, const std::string& pass, int practiceIdx)
{
    auto user = db.getUser(lpk, pass, practiceList[practiceIdx].rziCode);
    
    if (!user.has_value())
    {
        ModalDialogBuilder::showError("Грешно потребителско име или парола");
        return;
    }

    UserManager::setCurrentUser(user.value());

    loginSuccessful = true;

    view->closeLogin();

}
