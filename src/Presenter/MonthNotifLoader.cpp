#include "MonthNotifLoader.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "Network/PISServ.h"
#include <fstream>
#include <streambuf>

MonthNotifLoader::MonthNotifLoader(TabPresenter* presenter) :
    m_listHandler{ this },
    m_dataHandler{ this },
    presenter{presenter}
{}

void MonthNotifLoader::loadNotification()
{ 

    if (!UserManager::currentUser().isAdmin()) {
        ModalDialogBuilder::showMessage(u8"За да издавате фактури е необходимо да сте влезли като администратор");
        return;
    }

    auto dialogResult = ModalDialogBuilder::monthNotifLoadDialog();

    //dialog canceled
    if (dialogResult == MonthNotifLoad::Rejected) return;

    //load from file
    if (dialogResult == MonthNotifLoad::FromFile)
    {
        auto filepathResult = ModalDialogBuilder::getMonthNotifFromFile();

        if (!filepathResult.has_value()) return;

        std::ifstream t(filepathResult.value());

        presenter->openInvoice({
            (std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>() 
            });
        
        return;
    }

    if (m_awaitingReply) {
        return;
    }

    if (!m_notifRows.empty()) {
        setNotifRows(m_notifRows);
        return;
    }

    m_awaitingReply = PIS::sendRequest(
        SOAP::NotifList(UserManager::currentUser().practice.rziCode),
        m_listHandler
    );
}

void MonthNotifLoader::setMonthNotif(const std::string& monthNotif)
{
    m_awaitingReply = false;
    if (monthNotif.empty()) return;

    presenter->openInvoice(monthNotif);
}

void MonthNotifLoader::setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows)
{
    m_awaitingReply = false;

    if (!notifRows.has_value()) { return; }

    m_notifRows = notifRows.value();

    if (m_notifRows.empty()) {
        ModalDialogBuilder::showMessage(u8"Не са открити месечни известия");
        return;
    }

    auto idx = ModalDialogBuilder::monthNotifPicker(m_notifRows);

    if (idx == -1) {
        return;
    }
        
    //send the pis request for the hash
    auto& hash = m_notifRows[idx].hash;

    m_awaitingReply = PIS::sendRequest(
        SOAP::getNotificationData(UserManager::currentUser().practice.rziCode, hash),
        m_dataHandler
    );
}
