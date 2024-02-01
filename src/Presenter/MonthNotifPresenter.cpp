#include "MonthNotifPresenter.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter.h"
#include "View/Interfaces/IMonthNotifView.h"

MonthNotifPresenter::MonthNotifPresenter(){}


void MonthNotifPresenter::setView(IMonthNotifView* view)
{
    this->view = view;
    if (m_notifRows.size()) view->setMonthNotifRows(m_notifRows);
    
}

void MonthNotifPresenter::loadFromXml()
{
    if (!User::isAdmin()) {
        ModalDialogBuilder::showMessage("За да издавате фактури е необходимо да сте влезли като администратор");
        return;
    }

    auto fileData = ModalDialogBuilder::openFile();

    if (!fileData.has_value()) return;

    TabPresenter::get().openInvoice(fileData.value());

    return;
}

void MonthNotifPresenter::loadFromPis()
{

    m_listHandler.sendRequest(User::practice().rziCode,
        [=, this](auto result) {this->setNotifRows(result);}
    );
}

void MonthNotifPresenter::okPressed(int currentIdx)
{
    if (currentIdx == -1) return;
    
    m_notifHandler.sendRequest(User::practice().rziCode, m_notifRows[currentIdx].hash,
            [=, this](const std::string& result) {
            TabPresenter::get().openInvoice(result);
            view->closeParentDialog();
        });

}


void MonthNotifPresenter::setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows)
{
    if (!notifRows.has_value()) { return; }

    m_notifRows = notifRows.value();

    if (m_notifRows.empty()) {
        ModalDialogBuilder::showMessage("Не са открити месечни известия");
        return;
    }

    view->setMonthNotifRows(m_notifRows);



}
