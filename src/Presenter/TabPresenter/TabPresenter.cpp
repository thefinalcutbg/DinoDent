#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "ListInstance.h"
#include "../ListPresenter/ListPresenter.h"

TabPresenter::TabPresenter() : index_(-1), view(nullptr)
{
    listPresenter_.attachEditObserver(this);
}


void TabPresenter::editNotify()
{
    if (currentList()->edited) return;

    currentList()->edited = true;
    view->changeTabName(currentList()->getTabName());
}

bool TabPresenter::listExists(const Patient& patient)
{
    for (int i = 0; i < lists_.size(); i++)
    {
        if (lists_[i].patient->id == patient.id &&
            lists_[i].amb_list.date.month == Date::currentMonth() &&
            lists_[i].amb_list.date.year == Date::currentYear()
            )
        {
           
            view->focusTab(i);
            return true;
        }
    }

    return false;
}



void TabPresenter::setView(ITabView* view)
{
    this->view = view;
    view->setTabPresenter(this);
    listPresenter_.setView(view->listView());
}


std::shared_ptr<Patient> TabPresenter::getPatient_ptr(const Patient& patient)
{
    for (int i = 0; i < lists_.size()-1; i ++)
    {
        if (lists_[i].patient->id == patient.id)
            return lists_[i].patient;
    }

    return std::make_shared<Patient>(patient);
}

bool TabPresenter::listsExist(const std::string& ambList_id)
{
    for (int i = 0; i < lists_.size(); i++)
    {
        if (lists_[i].amb_list.id == ambList_id)
        {

            view->focusTab(i);
            return true;
        }
    }

    return false;
}

ListInstance* TabPresenter::currentList()
{
    if (index_ == -1 || index_ >= lists_.size()) return nullptr;

    return &lists_[index_];
}

void TabPresenter::setCurrentList(int index)
{
    if (index == -1)
    {
        //do something to the view??
        return;
    }

    if (currentList())
    {
        auto scroll = view->getScrollPos();
        currentList()->_scrollHeight = scroll.height;
        currentList()->_scrollWidth = scroll.width;
    }

    index_ = index;
    
    listPresenter_.setData(&lists_[index]);
    view->setScrollPos(ScrollPos{ currentList()->_scrollHeight, currentList()->_scrollWidth });
}
void TabPresenter::openList(const Patient& patient)
{

    if (listExists(patient)) return;

    lists_.emplace_back(); //creates the instance;

    auto& ambList = lists_.back().amb_list;

    amb_db.getListData(patient.id, Date::currentMonth(), Date::currentYear(), ambList),

    lists_.back().patient = getPatient_ptr(patient);
    
    if (ambList.isNew() && !patient.isAdult(ambList.date))
        ambList.charge = Charge::freed;
    else if (ambList.isNew() && patient.getAge(ambList.date) > 70)
        ambList.charge = Charge::retired;

    for (auto& m : ambList.procedures) //autofill NZOK procedures
        if (m.nzok)
            m.price = MasterNZOK::instance().getPatientPrice(m.code, ambList.date, 64, patient.isAdult(), ambList.full_coverage);

    view->newTab(lists_.size() - 1, lists_.back().getTabName());


}

#include "Model/AmbListRow.h"

void TabPresenter::openList(const AmbListRow& ambRow)
{
    for (int i = 0; i < lists_.size(); i++)
    {
        auto& openedList = lists_[i];

        if (openedList.amb_list.id == ambRow.id)
        {
            view->focusTab(i); return;
        }
    }

    lists_.emplace_back(); //creates the instance;

    auto& ambList = lists_.back().amb_list;

    amb_db.getListData(ambRow.id, ambList);

    Patient patient = patient_db.getPatient(ambRow.patientId);

    lists_.back().patient = getPatient_ptr(patient);


    for (auto& m : ambList.procedures) //autofill NZOK procedures
        if (m.nzok)
            m.price = MasterNZOK::instance().getPatientPrice(m.code, ambList.date, 64, patient.isAdult(), ambList.full_coverage);
    
    view->newTab(lists_.size() - 1, lists_.back().getTabName());
}

void TabPresenter::removeList(const std::string& ambID)
{
    for (int i = 0; i< lists_.size(); i++)
    {
        if (lists_[i].amb_list.id == ambID)
        {
            view->removeTab(i);
            lists_.erase(lists_.begin() + i);
        }
    }
}

void TabPresenter::removeCurrentList()
{
    if (index_ == -1) return;
    lists_.erase(lists_.begin() + index_);
    view->removeCurrentTab();
}
