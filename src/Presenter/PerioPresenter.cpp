#include "PerioPresenter.h"

#include <algorithm>

#include "Model/Dental/PerioToothData.h"
#include "Model/Dental/PerioStatistic.h"
#include "Model/Parser.h"

#include "Database/DbPerio.h"
#include "Database/DbAmbList.h"

#include "View/Widgets/TabView.h"
#include "View/ModalDialogBuilder.h"

#include "View/Graphics/PaintHint.h"

PerioPresenter::PerioPresenter(TabView* view, std::shared_ptr<Patient> patient, long long rowId) :
    TabInstance(view, TabType::PerioStatus, patient),
    view(view->perioView()),
    patient_info(view->perioView()->patientTile(), patient),
    m_perioStatus(rowId ? DbPerio::getPerioStatus(rowId) : DbPerio::getPerioStatus(patient->rowid, Date::currentDate())),
    m_toothStatus(DbAmbList::getStatus(patient->rowid, m_perioStatus.date))
{

    if (m_perioStatus.date != Date::currentDate() && !rowId) //if its not todays measurment and it is new document
    {

        m_perioStatus.rowid = 0; //clearing the id, because it's new measurment

        auto getPrevious = ModalDialogBuilder::askDialog
        ("Открито е предишно пародонтално измерване. "
            "Желаете ли да заредите старите резултати?");

        if (!getPrevious)
            m_perioStatus = PerioStatus();

        m_perioStatus.date = Date::currentDate();
    }


    for (auto& tooth : m_toothStatus)
    {

        if (
            tooth[Dental::Missing] ||
            tooth[Dental::Impacted] ||
            tooth[Dental::Implant]
            )
            m_perioStatus.disabled[tooth.index()] = true;


        if (tooth[Dental::Mobility])
            m_perioStatus.mobility[tooth.index()] =
            static_cast<int>(tooth.m_degree) + 1;
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

void PerioPresenter::teethViewChanged(bool showLower)
{
    m_showLowerTeeth = showLower;
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

void PerioPresenter::dateChanged(const Date& date)
{
    patient_info.setDate(date);
    m_perioStatus.date = date;
    edited = false;
    makeEdited();
}


long long PerioPresenter::rowID() const
{
    return m_perioStatus.rowid;
}

bool PerioPresenter::save()
{
    if (isNew()) {
        m_perioStatus.rowid = DbPerio::insertPerioStatus(m_perioStatus, patient->rowid);
    }
    else
    {
        DbPerio::updatePerioStatus(m_perioStatus);
    }

    refreshTabName();

    edited = false;

    return true;
}


bool PerioPresenter::isNew()
{
    return m_perioStatus.rowid == 0;
}

void PerioPresenter::print()
{
    view->print();
}

void PerioPresenter::setDataToView()
{
    view->setPresenter(this);

    view->setDate(m_perioStatus.date);

    for (int i = 0; i < 32; i++){
        view->setToothHint(ToothPaintHint(m_toothStatus[i]));
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

    patient_info.setDate(m_perioStatus.date);
    patient_info.setCurrent(true);


    view->showLowerTeeth(m_showLowerTeeth);

}

void PerioPresenter::prepareSwitch()
{
    TabInstance::prepareSwitch();
}

TabName PerioPresenter::getTabName()
{
    return isNew() ? 
    TabName{ 
        .header = "Нов пародонтален статус", 
        .footer = patient->firstLastName(), 
        .indicatorColor = patient->colorNameRgb 
    }
    :
    TabName{
        .header = "Пародонт. статус " + m_perioStatus.date.toBgStandard(),
        .footer = patient->firstLastName() ,
        .indicatorColor = patient->colorNameRgb
    };
}


PerioPresenter::~PerioPresenter()
{
    if(view != nullptr)
    view->setPresenter(nullptr);
}
