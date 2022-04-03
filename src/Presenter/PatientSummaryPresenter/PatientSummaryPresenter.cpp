#include "PatientSummaryPresenter.h"
#include "View/TabView/ITabView.h"
#include "View/PatientSummaryView/IPatientSummaryView.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include "Model/Patient.h"
#include <array>
#include <QDebug>
#include "Database/DbPatientSummary.h"
#include "Database/DbPerio.h"

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient)
    :   TabInstance(view, TabType::PatientSummary, patient), 
        view(view->summaryView()),
        m_currentFrameIdx{ 0 },
        statusTimeFrame(DbPatientSummary::getFrames(patient->rowid))
{
    auto perioStatuses = DbPerio::getAllPerioStatuses(patient->rowid);

    statusTimeFrame.reserve(statusTimeFrame.size() + perioStatuses.size());

    //inserting perioTimeFrames into their corresponding locations:

    for (int i = 0; i < perioStatuses.size(); i++) {
        TimeFrame t{
            TimeFrameType::Perio,
            perioStatuses[i].rowid,
            perioStatuses[i].LPK,
            perioStatuses[i].date,
            {},
            {},
            perioStatuses[i]

        };

        int instertAtIdx = 0;
        
        for (int y = 0; y < statusTimeFrame.size(); y++) {
            if (statusTimeFrame[y].date > t.date)
            {
                //getting the tooth status from previous frame
                if (y) {
                    t.teeth = statusTimeFrame[y - 1].teeth;
                }
                
                //the position at which the perio status frame should be inserted:
                instertAtIdx = y;
                break;
            }
        }

        statusTimeFrame.insert(statusTimeFrame.begin() + instertAtIdx, t);

    }

    //applying periostatus to all other timeframes
    for (int i = 0; i < statusTimeFrame.size(); i++) {

        if (statusTimeFrame[i].type == TimeFrameType::Perio) {
            
            for (int y = i + 1; y < statusTimeFrame.size(); y++) {
                
                if (statusTimeFrame[y].type == TimeFrameType::Perio) {
                    i = y;
                    break;
                }

                statusTimeFrame[y].perioData = statusTimeFrame[i].perioData;
            }
        }
    }



}


void PatientSummaryPresenter::setCurrentFrame(int index)
{
    m_currentFrameIdx = index;

    if (index < 0 || index >= statusTimeFrame.size()) return;

    auto& frame = statusTimeFrame[m_currentFrameIdx];

    view->setTeeth(ToothHintCreator::getTeethHint(statusTimeFrame[m_currentFrameIdx].teeth));

    /* //FOR LATER:
    switch (frame.type)
    {
    case::TimeFrameType::Procedures:
   
        break;
    case::TimeFrameType::Perio:
        
        break;
      
    }
    */

    view->setProcedures(frame.procedures);
    view->setPerioData(PerioWithDisabled(frame.perioData));


    view->setDateLabel("Дата: " + statusTimeFrame[m_currentFrameIdx].date.toString());

}

long long PatientSummaryPresenter::rowID() const
{
    return patient->rowid;
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
