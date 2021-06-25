#include "TabPresenter.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "ListInstance.h"
#include "../ListPresenter/ListPresenter.h"

TabPresenter::TabPresenter() : _index(-1), view(nullptr)
{
    _listPresenter.attachEditObserver(this);
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
    _listPresenter.setView(view->listView());
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
    if (_index == -1 || _index >= _lists.size()) return nullptr;

    return &_lists[_index];
}
#include <QDebug>
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

    _index = index;
    
    _listPresenter.setData(&_lists[index]);
    qDebug() << "h/w set:" << currentList()->_scrollHeight << currentList()->_scrollWidth;
    view->setScrollPos(ScrollPos{ currentList()->_scrollHeight, currentList()->_scrollWidth });
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

void TabPresenter::removeCurrentList()
{
    if (_index == -1) return;
    _lists.erase(_lists.begin() + _index);
    view->removeCurrentTab();
}
