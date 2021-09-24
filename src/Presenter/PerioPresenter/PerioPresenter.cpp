#include "PerioPresenter.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"

PerioPresenter::PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient) :
    TabInstance(view, TabType::PerioList), 
    patient(patient), 
    view(view->perioView()),
    perioDate(Date::currentDate()),
    m_toothStatus(m_db.getStatus(patient->id, perioDate))
{
    for (auto& tooth : m_toothStatus)
    {

        auto& perioStatus = tooth.index < 16 ? m_perioUpper : m_perioLower;
        int idx = tooth.index < 16 ? tooth.index : 31 - tooth.index;

        if (tooth.extraction.exists())
            perioStatus.disabled[idx] = true;

        if (tooth.mobility.exists())
            perioStatus.mobility[idx] = static_cast<int>(tooth.mobility.degree) + 1;

    }


}

bool PerioPresenter::save()
{
    return true;
}

bool PerioPresenter::saveAs()
{
    return true;
}

bool PerioPresenter::isNew()
{
    return false;
}

void PerioPresenter::print()
{
}

void PerioPresenter::setCurrent()
{
    view->setPresenter(this);
    _tabView->showPerioView();

    for (int i = 0; i < 32; i++){
        view->setToothHint(ToothHintCreator::getToothHint(m_toothStatus[i]));
    }
       
    view->setPerioStatus(m_perioUpper, m_perioLower);

    setScrollPosition();

}

void PerioPresenter::prepareSwitch()
{
    auto& [up, low] = view->getPerioStatus();

    m_perioUpper = up;
    m_perioLower = low;

    TabInstance::prepareSwitch();
}

std::string PerioPresenter::getTabName()
{
    return u8"Пародонтално измерване " + patient->firstLastName() + " " + Date::toString(perioDate);
}
