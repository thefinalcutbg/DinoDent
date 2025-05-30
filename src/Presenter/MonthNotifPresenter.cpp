﻿#include "MonthNotifPresenter.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter.h"
#include "View/Interfaces/IMonthNotifView.h"
#include "Database/DbInvoice.h"

MonthNotifPresenter::MonthNotifPresenter(){}


void MonthNotifPresenter::setView(IMonthNotifView* view)
{
    this->view = view;
    if (m_notifRows.empty()) return;
    
    auto claimedNotifs = DbInvoice::getClaimedHashes();

    for (auto& n : m_notifRows) {
        n.is_claimed = claimedNotifs.count(n.claimedHash);
    }
    
    view->setMonthNotifRows(m_notifRows);
    
}

void MonthNotifPresenter::loadFromXml()
{
    if (!User::isAdmin()) {
        ModalDialogBuilder::showMessage("За да издавате фактури е необходимо да сте влезли като администратор");
        return;
    }

    auto fileData = ModalDialogBuilder::openFile();

    if (!fileData.has_value()) return;

    TabPresenter::get().openInvoice(fileData.value(), "default");

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

            if(result.empty()) {
                return;
            }
            TabPresenter::get().openInvoice(result, m_notifRows[currentIdx].claimedHash);
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

    auto claimedNotifs = DbInvoice::getClaimedHashes();

    for (auto& n : m_notifRows) {
        n.is_claimed = claimedNotifs.count(n.claimedHash);
    }

    view->setMonthNotifRows(m_notifRows);
}
