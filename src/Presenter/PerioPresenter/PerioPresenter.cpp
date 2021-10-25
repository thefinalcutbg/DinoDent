#include "PerioPresenter.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include <algorithm>
#include "Model/PerioToothData.h"
#include "Model/PerioStatistic.h"

PerioPresenter::PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient) :
    TabInstance(view, TabType::PerioList), 
    patient(patient), 
    view(view->perioView()),
    perioDate(Date::currentDate()),
    m_toothStatus(m_db.getStatus(patient->id, perioDate))
{
    for (auto& tooth : m_toothStatus)
    {

        if (tooth.extraction.exists() || tooth.impacted.exists())
            m_perioStatus.disabled[tooth.index] = true;

        if (tooth.mobility.exists())
            m_perioStatus.mobility[tooth.index] = static_cast<int>(tooth.mobility.degree) + 1;
    }


}

void PerioPresenter::toothButtonClicked(int tooth)
{
    auto& disabled = m_perioStatus.disabled[tooth];
    disabled = !disabled;

    disabled == true ?
        view->disableTooth(tooth)
        :
        view->setToothData(PerioToothData(m_perioStatus, tooth));

    PerioStatistic stat(m_perioStatus);
    view->setPerioStatistic(stat);

}


void PerioPresenter::refreshMeasurment(int index)
{
    view->setMeasurment(
        index,
        m_perioStatus.pd[index],
        m_perioStatus.cal[index],
        m_perioStatus.gm[index],
        getRecession(index)
    );

    PerioStatistic stat(m_perioStatus);
    view->setPerioStatistic(stat);
}

int PerioPresenter::getRecession(int surfaceIndex)
{
    int recIdx{ surfaceIndex / 3 }, recession{ 0 };

    for (int i = recIdx * 3; i < recIdx * 3 + 3; i++)
    {
        int recTemp = m_perioStatus.gm[i] * (-1);
        recession = std::max(recession, recTemp);
    }

    return recession;
}


void PerioPresenter::pdChanged(int index, int value)
{
    m_perioStatus.pd[index] = value;
    m_perioStatus.gm[index] = value - m_perioStatus.cal[index];

    refreshMeasurment(index);
}

void PerioPresenter::calChanged(int index, int value)
{
    m_perioStatus.cal[index] = value;
    m_perioStatus.gm[index] = m_perioStatus.pd[index] - value;

    refreshMeasurment(index);
}

void PerioPresenter::gmChanged(int index, int value)
{
    auto& gingivalMargin = m_perioStatus.gm[index];
    auto& CAL = m_perioStatus.cal[index];
    auto& pocketDepth = m_perioStatus.pd[index];

    gingivalMargin = value;

    if (gingivalMargin > pocketDepth) pocketDepth = value;
    
    int calTemp = pocketDepth - value;

    constexpr int calMax = 19;

    if (calTemp <= calMax) CAL = calTemp;
    else pocketDepth = CAL + gingivalMargin;

    refreshMeasurment(index);
}

void PerioPresenter::bopChanged(int index, bool checked)
{
    m_perioStatus.bop[index] = checked;
}

void PerioPresenter::attachChanged(int index, int value)
{
    m_perioStatus.ag[index] = value;
}

void PerioPresenter::FMBSChanged(int index, bool value)
{
    m_perioStatus.FMBS[index] = value;

    PerioStatistic stat(m_perioStatus);
    view->setPerioStatistic(stat);

}

void PerioPresenter::FMPSChanged(int index, bool value)
{
    m_perioStatus.FMPS[index] = value;

    PerioStatistic stat(m_perioStatus);
    view->setPerioStatistic(stat);

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
       
    
    for (int i = 0; i < 32; i++)
    {
        m_perioStatus.disabled[i] ?

            view->disableTooth(i) 
            :
            view->setToothData(PerioToothData(m_perioStatus, i));
        
    }

    PerioStatistic stat(m_perioStatus);
    view->setPerioStatistic(stat);

    setScrollPosition();

}

void PerioPresenter::prepareSwitch()
{
    TabInstance::prepareSwitch();
}

std::string PerioPresenter::getTabName()
{
    return u8"Пародонтален статус " + patient->firstLastName() + " " + Date::toString(perioDate);
}
