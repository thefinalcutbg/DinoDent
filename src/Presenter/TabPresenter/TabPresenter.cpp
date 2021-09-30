#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/AmbListRow.h"
#include "../ListPresenter/ListPresenter.h"
#include "../PerioPresenter/PerioPresenter.h"


TabPresenter::TabPresenter() : _indexCounter(-1), m_currentIndex(-1), view(nullptr)
{
    m_tabs.reserve(20);
}

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

        if (tabInstance->type != TabType::AmbList) continue;

        auto listPresenter = static_cast<ListPresenter*>(tabInstance);

        if (listPresenter->patient->id == patient.id)
            return listPresenter->patient;
    }

    return std::make_shared<Patient>(patient);
}


void TabPresenter::openList(const Patient& patient)
{
    if (newListExists(patient)) return;

    TabInstance* newTab = new ListPresenter(view, getPatient_ptr(patient));

    openTab(newTab);

}

void TabPresenter::openList(const AmbListRow& ambRow)
{

    if (listsExists(ambRow.id)) return;

    TabInstance* newTab = new ListPresenter(
        view, 
        getPatient_ptr(patient_db.getPatient(ambRow.patientId)), 
            ambRow.id);

    openTab(newTab);

}

void TabPresenter::openPerio(const Patient& patient)
{
    TabInstance* newTab = new PerioPresenter(view, getPatient_ptr(patient));

    openTab(newTab);
}



bool TabPresenter::listsExists(const std::string& ambList_id)
{
    for (auto& [index, tabInstance] : m_tabs)
    {

        if (tabInstance->type != TabType::AmbList) continue;

        auto listPresenter = static_cast<ListPresenter*>(tabInstance);
        if (listPresenter->m_ambList.id == ambList_id)
        {
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

        if (listPresenter->patient->id == patient.id &&
            listPresenter->m_ambList.date.month == Date::currentMonth() &&
            listPresenter->m_ambList.date.year == Date::currentYear()
            )
        {

            view->focusTab(index);
            return true;
        }
    }

    return false;
}


void TabPresenter::removeList(const std::string& ambID)
{

    for (const auto& [index, tab] : m_tabs)
    {
        if (tab->type != TabType::AmbList) continue;

        if (static_cast<ListPresenter*>(tab)->m_ambList.id == ambID)
        {
            setCurrentTab(index);
            removeCurrentTab();
        }
            
    }

}
