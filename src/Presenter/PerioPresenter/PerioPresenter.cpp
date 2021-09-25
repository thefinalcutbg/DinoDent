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

        if (tooth.extraction.exists())
            m_perioStatus.disabled[tooth.index] = true;

        if (tooth.mobility.exists())
            m_perioStatus.mobility[tooth.index] = static_cast<int>(tooth.mobility.degree) + 1;
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
       
    view->setPerioStatus(m_perioStatus);

    setScrollPosition();

}

void PerioPresenter::prepareSwitch()
{
    m_perioStatus = view->getPerioStatus();

    TabInstance::prepareSwitch();
}

std::string PerioPresenter::getTabName()
{
    return u8"Пародонтален статус " + patient->firstLastName() + " " + Date::toString(perioDate);
}
