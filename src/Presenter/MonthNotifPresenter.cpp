#include "MonthNotifPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/TabPresenter.h"
#include "View/Interfaces/IMonthNotifView.h"
#include <fstream>
#include <streambuf>

MonthNotifPresenter::MonthNotifPresenter(TabPresenter* presenter) :
    tab_presenter{presenter}, view{nullptr}
{}


void MonthNotifPresenter::setView(IMonthNotifView* view)
{
    this->view = view;
    if (m_notifRows.size()) view->setMonthNotifRows(m_notifRows);
    
}

void MonthNotifPresenter::loadFromXml()
{
    if (!User::isAdmin()) {
        ModalDialogBuilder::showMessage(u8"За да издавате фактури е необходимо да сте влезли като администратор");
        return;
    }

    auto filepathResult = ModalDialogBuilder::getMonthNotifFromFile();

    if (!filepathResult.has_value()) return;

    std::ifstream t(filepathResult.value());

    view->closeParentDialog();

    tab_presenter->openInvoice({
        (std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>()
        });

    return;
}

void MonthNotifPresenter::loadFromPis()
{
    if (!m_notifRows.empty()) {
        setNotifRows(m_notifRows);
        return;
    }

    m_listHandler.sendRequest(User::practice().rziCode,
        [=](auto result) {this->setNotifRows(result);}
    );
}

void MonthNotifPresenter::okPressed(int currentIdx)
{
    if (currentIdx == -1) return;

    view->closeParentDialog();
    
    m_notifHandler.sendRequest(User::practice().rziCode, m_notifRows[currentIdx].hash,
            [=](const std::string& result) { 
            tab_presenter->openInvoice(result);
        });

}


void MonthNotifPresenter::setNotifRows(const std::optional<std::vector<MonthNotifRow>>& notifRows)
{
    if (!notifRows.has_value()) { return; }

    m_notifRows = notifRows.value();

    if (m_notifRows.empty()) {
        ModalDialogBuilder::showMessage(u8"Не са открити месечни известия");
        return;
    }

    view->setMonthNotifRows(m_notifRows);



}
