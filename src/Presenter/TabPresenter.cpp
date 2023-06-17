#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/TableRows.h"
#include "Database/DbPatient.h"
#include "ListPresenter.h"
#include "PerioPresenter.h"
#include "PatientSummaryPresenter.h"
#include "PrescriptionPresenter.h"
#include "FinancialPresenter.h"
#include "StatisticDialogPresenter.h"
#include <TinyXML/tinyxml.h>

TabPresenter TabPresenter::s_singleton;

void TabPresenter::setView(ITabView* view)
{
    this->view = view;
}

TabInstance* TabPresenter::currentTab()
{
    if (!m_tabs.count(m_currentIndex))
    {
        return nullptr;
    }

    return m_tabs[m_currentIndex];
}


void TabPresenter::createNewTab(TabInstance* tabInstance, bool setFocus)
{
    m_indexCounter++;

    m_tabs[m_indexCounter] = tabInstance;

    tabInstance->setContainerIdx(m_indexCounter);

    view->newTab(m_indexCounter, m_tabs[m_indexCounter]->getTabName());

    //tabs size() > 1, because the first tab gets focused by the view automatically
    if (setFocus && m_tabs.size() > 1)
        view->focusTab(m_indexCounter);
}

void TabPresenter::setCurrentTab(int index)
{
    if (currentTab() != nullptr) currentTab()->prepareSwitch();

    m_currentIndex = index;

    if (index == -1)
    {
        //it's 100% empty, but just in case...
        if (m_tabs.empty()) {
            m_indexCounter = -1;
        }

        view->showWelcomeScreen();
        return;
    }

    m_tabs[index]->setCurrent();
    
}


void TabPresenter::closeTabRequested(int tabId)
{
    if (!m_tabs[tabId]->premissionToClose()) return;

    delete m_tabs[tabId];
    m_tabs.erase(tabId);
    view->removeTab(tabId);
}


bool TabPresenter::permissionToLogOut()
{
    for (auto& pair : m_tabs)
    {
        if (!m_tabs[pair.first]->premissionToClose())
            return false;
    }

    m_tabs.clear();
    view->removeAllTabs();

    return true;
}


std::shared_ptr<Patient> TabPresenter::getPatient_ptr(const Patient& patient)
{
    for (auto& [index, tabInstance] : m_tabs)
    {
        if (!tabInstance->patient) continue;

        if(tabInstance->patient->rowid == patient.rowid) return tabInstance->patient;
    }

    auto result = std::make_shared<Patient>(patient);

    result->teethNotes = DbPatient::getPresentNotes(result->rowid);

    return result;
}


void TabPresenter::refreshPatientTabNames(long long patientRowId)
{
    for (auto tab : m_tabs) {

        if (tab.second->patient == nullptr ||
            tab.second->patient->rowid != patientRowId) {
            continue;
        }

        view->changeTabName(tab.second->getTabName(), tab.first);
    }
}

void TabPresenter::openList(const Patient& patient)
{
    if (newListExists(patient)) return;

    createNewTab(new ListPresenter(view, getPatient_ptr(patient)));
    
}

void TabPresenter::openPerio(const Patient& patient)
{
    createNewTab(new PerioPresenter(view, getPatient_ptr(patient)));
}

void TabPresenter::openPerscription(const Patient& patient)
{
    createNewTab(new PrescriptionPresenter(view, getPatient_ptr(patient)));
}

void TabPresenter::openInvoice(const std::string& monthNotif)
{
    try {
        auto presenter = new FinancialPresenter(view, monthNotif);

        if (monthNotiAlreadyOpened(presenter->m_invoice.nhifData->fin_document_month_no)) {
            delete presenter;
            return;
        }

        createNewTab(presenter);
    }
    catch(const std::exception& e) {
        ModalDialogBuilder::showError(e.what());
    }

    
}

void TabPresenter::openInvoice(long long patientRowId, const std::vector<Procedure>& procedures)
{
    
    
    createNewTab(new FinancialPresenter(
                view, 
                getPatient_ptr(
                    DbPatient::get(patientRowId)
                ),
                procedures
            )
    );
}


void TabPresenter::open(const RowInstance& row, bool setFocus)
{
    if (tabAlreadyOpened(row)) return;

    TabInstance* newTab{nullptr};

    switch (row.type)
    {
    case TabType::AmbList:
        newTab = new ListPresenter(view, getPatient_ptr(DbPatient::get(row.patientRowId)), row.rowID);
        break;
    case TabType::PerioStatus:
        newTab = new PerioPresenter(view, getPatient_ptr(DbPatient::get(row.patientRowId)), row.rowID);
        break;
    case TabType::PatientSummary:
        newTab = new PatientSummaryPresenter(view, getPatient_ptr(DbPatient::get(row.patientRowId)));
        break;
    case TabType::Financial:
        newTab = row.rowID ? 
            new FinancialPresenter(view, row.rowID)
            :
            new FinancialPresenter(view, getPatient_ptr(DbPatient::get(row.patientRowId)));
        break;
    case TabType::Prescription:
        newTab = new PrescriptionPresenter(view, getPatient_ptr(DbPatient::get(row.patientRowId)), row.rowID);
        break;
    }

    createNewTab(newTab, setFocus);
}

bool TabPresenter::tabAlreadyOpened(const RowInstance& row)
{

    for (auto& [index, tab] : m_tabs)
    {
        if (tab->type == row.type && 
            tab->rowID() == row.rowID &&
            (tab->patient == nullptr || //financial tab edge case
            tab->patient->rowid == row.patientRowId )
        )
        {
            view->focusTab(index);
            return true;
        }
    }

    return false;
}

bool TabPresenter::monthNotiAlreadyOpened(int monthNotifNum)
{
    for (auto& [index, tabInstance] : m_tabs)
    {
        if (tabInstance->type != TabType::Financial) continue;

        auto finPresenter = static_cast<FinancialPresenter*>(tabInstance);

        if (finPresenter->m_invoice.nhifData.has_value() &&
            finPresenter->m_invoice.nhifData->fin_document_month_no == monthNotifNum) {

            view->focusTab(index);
            return true;
        }
    }

    return false;
}

bool TabPresenter::newListExists(const Patient& patient)
{

    for (auto& [index, tabInstance] : m_tabs)
    {

        if (tabInstance->type != TabType::AmbList) continue;

        auto listPresenter = static_cast<ListPresenter*>(tabInstance);

        auto ambSheetDate = listPresenter->m_ambList.getDate();

        if (listPresenter->patient->id == patient.id &&
            ambSheetDate.month == Date::currentMonth() &&
            ambSheetDate.year == Date::currentYear()
            )
        {

            view->focusTab(index);
            return true;
        }
    }

    return false;
}



bool TabPresenter::documentTabOpened(TabType type, long long rowID) const
{
    for (const auto& [index, tab] : m_tabs)
    {

        if (tab->type == type && tab->rowID() == rowID)
        {
            return true;
        }
    }

    //if user wants to delete the patient, check other procedureType of documents related to the patient
    if (type == TabType::PatientSummary)
    {
        for (const auto& [index, tab] : m_tabs)
        {       
            //the financial tabs patient ptr is always null
            if (tab->type != TabType::Financial &&
                tab->patient.get()->rowid == rowID
                )
            {
                return true;
            }
        }
    }

    return false;
}

bool TabPresenter::patientTabOpened(const long long patientRowid) const
{
   

    return false;
}
