#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/TableRows.h"
#include "Database/DbPatient.h"
#include "../ListPresenter/ListPresenter.h"
#include "../PerioPresenter/PerioPresenter.h"
#include "../PatientSummaryPresenter/PatientSummaryPresenter.h"
#include "../FinancialPresenter/FinancialPresenter.h"
#include "Libraries/TinyXML/tinyxml.h"

TabPresenter::TabPresenter() : _indexCounter(-1), m_currentIndex(-1), view(nullptr)
{}

void TabPresenter::setView(ITabView* view)
{
    this->view = view;
    view->setTabPresenter(this);
}

TabInstance* TabPresenter::currentTab()
{
    if (!m_tabs.count(m_currentIndex))
    {
        return nullptr;
    }

    return m_tabs[m_currentIndex];
}


void TabPresenter::openTab(TabInstance* tabInstance)
{
    _indexCounter++;

    m_tabs[_indexCounter] = tabInstance;

    view->newTab(_indexCounter, m_tabs[_indexCounter]->getTabName());

    view->focusTab(_indexCounter);
    setCurrentTab(_indexCounter);
}

void TabPresenter::setCurrentTab(int index)
{

    if (currentTab() != nullptr) currentTab()->prepareSwitch();

    m_currentIndex = index;

    if (index == -1)
    {
        view->showDinosaur();
        return;
    }

    m_tabs[index]->setCurrent();
}

void TabPresenter::removeCurrentTab()
{
    delete m_tabs[m_currentIndex];
    m_tabs.erase(m_currentIndex);
    view->removeCurrentTab();
}


std::shared_ptr<Patient> TabPresenter::getPatient_ptr(const Patient& patient)
{
    for (auto& [index, tabInstance] : m_tabs)
    {
        if (tabInstance->patient != nullptr && 
            tabInstance->patient->id == patient.id
            )
            return tabInstance->patient;
    }

    auto result = std::make_shared<Patient>(patient);


    result->teethNotes = DbPatient::getPresentNotes(result->id);

    return result;
}


void TabPresenter::openList(const Patient& patient)
{
    if (newListExists(patient)) return;

    openTab(new ListPresenter(view, this, getPatient_ptr(patient)));
    
}

void TabPresenter::openPerio(const Patient& patient)
{
    openTab(new PerioPresenter(view, getPatient_ptr(patient)));
}

void TabPresenter::openInvoice(const std::string& monthNotifFilePath)
{
    try {
        auto presenter = new FinancialPresenter(view, monthNotifFilePath);

        if (monthNotiAlreadyOpened(presenter->m_invoice.nzokData->fin_document_month_no))
            return;

        openTab(presenter);
    }
    catch(const std::exception& e) {
        ModalDialogBuilder::showError(e.what());
    }

    
}

void TabPresenter::openInvoice(const Procedures& procedures, std::shared_ptr<Patient> patient)
{
    openTab(new FinancialPresenter(view, procedures, patient));
}

void TabPresenter::open(const RowInstance& row)
{
    if (tabAlreadyOpened(row.type, row.rowID)) return;

    TabInstance* newTab{nullptr};

    switch (row.type)
    {
    case TabType::AmbList:
        newTab = new ListPresenter(view, this, getPatient_ptr(DbPatient::getPatient(row.patientId)), row.rowID);
        break;
    case TabType::PerioList:
        newTab = new PerioPresenter(view, getPatient_ptr(DbPatient::getPatient(row.patientId)), row.rowID);
        break;
    case TabType::PatientSummary:
        newTab = new PatientSummaryPresenter(view, getPatient_ptr(DbPatient::getPatient(row.patientId)));
        break;
    case TabType::Financial:
        newTab = new FinancialPresenter(view, std::stoi(row.rowID));
        break;
    }

    openTab(newTab);
}

bool TabPresenter::tabAlreadyOpened(TabType type, const std::string& rowID)
{
    for (auto& [index, tabInstance] : m_tabs)
    {
        if (tabInstance->type == type && tabInstance->rowID() == rowID)
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

        if (finPresenter->m_invoice.nzokData.has_value() &&
            finPresenter->m_invoice.nzokData->fin_document_month_no == monthNotifNum) {

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

        auto ambSheetDate = listPresenter->m_ambList.getAmbListDate();

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

void TabPresenter::removeTab(TabType type, const std::string& rowID)
{
    for (const auto& [index, tab] : m_tabs)
    {
        if (tab->type == type && tab->rowID() == rowID)
        {
            view->focusTab(index);
            removeCurrentTab();
            return;
        }
    }
}

void TabPresenter::removePatientTabs(const std::string& patientID)
{
    for (const auto& [index, tab] : m_tabs)
    {
        if (tab->patient != nullptr && 
            tab->patient.get()->id == patientID
        )
        {
            view->focusTab(index);
            removeCurrentTab();
        }
    }
}
