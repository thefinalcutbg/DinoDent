#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "ListInstance.h"
#include "../ListPresenter/ListPresenter.h"

TabPresenter::TabPresenter() : _index(-1), view(nullptr), listPresenter(nullptr)
{

}

void TabPresenter::editNotify()
{
    if (currentList()->edited) return;

    currentList()->edited = true;
    view->changeTabName(currentList()->getTabName());
}

bool TabPresenter::listExists(const Patient& patient)
{
    for (int i = 0; i < _lists.size(); i++)
    {
        if (_lists[i].patient->id == patient.id &&
            _lists[i].amb_list.date.month == Date::currentMonth() &&
            _lists[i].amb_list.date.year == Date::currentYear()
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
}

void TabPresenter::setListPresenter(ListPresenter* listPresenter)
{
    this->listPresenter = listPresenter;
    listPresenter->attachEditObserver(this);
}


std::shared_ptr<Patient> TabPresenter::getPatient_ptr(const Patient& patient)
{
    for (int i = 0; i < _lists.size()-1; i ++)
    {
        if (_lists[i].patient->id == patient.id)
            return _lists[i].patient;
    }

    return std::make_shared<Patient>(patient);
}

bool TabPresenter::listsExist(const std::string& ambList_id)
{
    for (int i = 0; i < _lists.size(); i++)
    {
        if (_lists[i].amb_list.id == ambList_id)
        {

            view->focusTab(i);
            return true;
        }
    }

    return false;
}



ListInstance* TabPresenter::currentList()
{
    if (_index == -1) return nullptr;

    return &_lists[_index];
}

void TabPresenter::setCurrentList(int index)
{
    _index = index;
    
    if (index == -1) return;

    listPresenter->setData(&_lists[index]);
}
void TabPresenter::newList(const Patient& patient)
{

    if (listExists(patient)) return;

    _lists.emplace_back(); //creates the instance;

    auto& ambList = _lists.back().amb_list;

    database.getListData(patient.id, Date::currentMonth(), Date::currentYear(), ambList),

    _lists.back().patient = getPatient_ptr(patient);
    
    for (auto& m : ambList.manipulations) //autofill NZOK procedures
    {
        if (m.nzok)
        {
            auto [patient_price, nzok_price] = MasterNZOK::instance().getPrices(m.code, ambList.date, 64, patient.isAdult(), ambList.unfavourable);
            m.price = patient_price;
        }
    }

    view->newTab(_lists.size() - 1, _lists.back().getTabName());


}

void TabPresenter::closeList()
{
    _lists.erase(_lists.begin() + _index);
    view->removeCurrentTab();
}
