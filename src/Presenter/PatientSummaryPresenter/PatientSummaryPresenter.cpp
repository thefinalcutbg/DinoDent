#include "PatientSummaryPresenter.h"
#include "View/TabView/ITabView.h"
#include "View/PatientSummaryView/IPatientSummaryView.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include "Model/Patient.h"
#include <array>
#include <QDebug>

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient)
    :   TabInstance(view, TabType::PatientSummary, patient), 
        view(view->summaryView()),
        m_currentFrameIdx{ 0 },
        statusTimeFrame(m_db.getFrames(patient->id))
{}


void PatientSummaryPresenter::setCurrentFrame(int index)
{
    m_currentFrameIdx = index;
    view->setTeeth(ToothHintCreator::getTeethHint(statusTimeFrame[m_currentFrameIdx].teeth));
    view->setProcedures(statusTimeFrame[m_currentFrameIdx].procedures);
    
    m_currentFrameIdx ?
        view->setDateLabel("Дата: "+ statusTimeFrame[m_currentFrameIdx].date.toString())
        :
        view->setDateLabel(u8"Начален статус");
}

const std::string& PatientSummaryPresenter::rowID() const
{
    return patient->id;
}


void PatientSummaryPresenter::print()
{

}


void PatientSummaryPresenter::setCurrent()
{
    view->setPresenter(this);
    _tabView->showSummaryView();

    view->setPatient(*patient.get());
    view->setTimeFrameCount(statusTimeFrame.size());
    view->setTickPosition(m_currentFrameIdx);  
    setCurrentFrame(m_currentFrameIdx);
}



TabName PatientSummaryPresenter::getTabName()
{
    return{
        u8"Пациент",
        this->patient->firstLastName()
    };
}

PatientSummaryPresenter::~PatientSummaryPresenter()
{
    if (view != nullptr) view->setPresenter(nullptr);
}
