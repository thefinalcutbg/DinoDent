#include "PerioPresenter.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include <algorithm>
#include "Model/PerioToothData.h"
#include "Model/PerioStatistic.h"
#include "Model/Parser/Parser.h"
#include "View/ModalDialogBuilder.h"


#include <QDebug>

PerioPresenter::PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient) :
    TabInstance(view, TabType::PerioList), 
    patient(patient), 
    view(view->perioView()),
    m_toothStatus(m_db.getStatus(patient->id, Date::currentDate())),
    m_perioStatus(m_db.getPerioStatus(patient->id, Date::currentDate()))
{

    qDebug() << "periostatus id: " << QString::fromStdString(m_perioStatus.id);

    if (m_perioStatus.date != Date::currentDate()) //if its not todays measurment
    {

        m_perioStatus.id.clear(); //clearing the id, because it's new measurment

        auto getPrevious = ModalDialogBuilder::askDialog
        (u8"Открито е предишно пародонтално измерване. "
            u8"Желаете ли да заредите старите резултати?");

        if(!getPrevious)
            m_perioStatus = PerioStatus();

        
    }


    for (auto& tooth : m_toothStatus)
    {

        m_perioStatus.disabled[tooth.index] = (
                                                tooth.extraction.exists() ||
                                                tooth.impacted.exists() ||
                                                tooth.implant.exists()
                                              );

        if (tooth.mobility.exists())
            m_perioStatus.mobility[tooth.index] = 
                                        static_cast<int>(tooth.mobility.degree) + 1;
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

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();

}


void PerioPresenter::refreshMeasurment(int index)
{
    view->setMeasurment(
        index,
        m_perioStatus.pd[index],
        m_perioStatus.cal[index],
        m_perioStatus.pd[index] - m_perioStatus.cal[index],
        getRecession(index)
    );

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

}

int PerioPresenter::getRecession(int surfaceIndex)
{
    int recIdx{ surfaceIndex / 3 }, recession{ 0 };

    for (int i = recIdx * 3; i < recIdx * 3 + 3; i++)
    {
        int recTemp = (m_perioStatus.pd[i] - m_perioStatus.cal[i]) * (-1);
        recession = std::max(recession, recTemp);
    }

    return recession;
}


void PerioPresenter::pdChanged(int index, int value)
{
    m_perioStatus.pd[index] = value;

    refreshMeasurment(index);

    makeEdited();
}

void PerioPresenter::calChanged(int index, int value)
{
    m_perioStatus.cal[index] = value;


    refreshMeasurment(index);

    makeEdited();
}

void PerioPresenter::gmChanged(int index, int value)
{
    auto& CAL = m_perioStatus.cal[index];
    auto& pocketDepth = m_perioStatus.pd[index];

    int gingivalMargin = value;

    if (gingivalMargin > pocketDepth) pocketDepth = value;
    
    int calTemp = pocketDepth - value;

    constexpr int calMax = 19;

    if (calTemp <= calMax) CAL = calTemp;
    else pocketDepth = CAL + gingivalMargin;

    refreshMeasurment(index);

    makeEdited();
}

void PerioPresenter::bopChanged(int index, bool checked)
{
    m_perioStatus.bop[index] = checked;

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();
}

void PerioPresenter::attachChanged(int index, int value)
{
    m_perioStatus.ag[index] = value;

    makeEdited();
}

void PerioPresenter::FMBSChanged(int index, bool value)
{
    m_perioStatus.FMBS[index] = value;

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();

}

void PerioPresenter::FMPSChanged(int index, bool value)
{
    m_perioStatus.FMPS[index] = value;

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();

}

void PerioPresenter::furcationChanged(int index, int a, int b, int c)
{
    int idx = index * 3;
    m_perioStatus.furcation[idx] = a;
    m_perioStatus.furcation[idx + 1] = b;
    m_perioStatus.furcation[idx + 2] = c;

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();
}

void PerioPresenter::smokeClicked(int value)
{
    m_perioStatus.smoker = value;
    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();
}

void PerioPresenter::boneLossChanged(int value)
{
    m_perioStatus.boneLoss = value;
    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();
}

void PerioPresenter::systemicChanged(bool enabled)
{
    m_perioStatus.systemic = enabled;
    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    makeEdited();
}

void PerioPresenter::restorationChanged(bool enabled)
{
    m_perioStatus.completeRestorationNeeded = enabled;
    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));
 
    makeEdited();
}


bool PerioPresenter::save()
{
    
    if(isNew())
        return saveAs();

    m_db.updatePerioStatus(m_perioStatus);
    _tabView->changeTabName(getTabName());
    edited = false;

    return true;
}

bool PerioPresenter::saveAs()
{

    m_db.insertPerioStatus(m_perioStatus, patient->id);
    _tabView->changeTabName(getTabName());
    edited = false;

    return true;
}

bool PerioPresenter::isNew()
{
    return m_perioStatus.id.empty();
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

    view->setPerioStatistic(PerioStatistic(m_perioStatus, patient->getAge(m_perioStatus.date)));

    view->setAdditional(
            m_perioStatus.smoker, 
            m_perioStatus.boneLoss, 
            m_perioStatus.systemic, 
            m_perioStatus.completeRestorationNeeded
    );

    setScrollPosition();

}

void PerioPresenter::prepareSwitch()
{
    TabInstance::prepareSwitch();
}

std::string PerioPresenter::getTabName()
{
    return u8"Пародонтален статус " + patient->firstLastName() + " " + Date::toString(m_perioStatus.date);
}