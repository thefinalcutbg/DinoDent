#include <array>

#include "PatientSummaryPresenter.h"
#include "View/TabView/ITabView.h"
#include "View/PatientSummaryView/IPatientSummaryView.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include "Model/Patient.h"
#include "Database/DbPatientSummary.h"
#include "Database/DbPerio.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Model/PerioStatistic.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "Model/TableRows.h"
#include "Model/User/User.h"


TimeFrame* PatientSummaryPresenter::currentFrame()
{
    if (m_currentFrameIdx < 0 ||
        m_currentFrameIdx >= statusTimeFrame.size()) 
    {
        return nullptr;
    }

    return &statusTimeFrame[m_currentFrameIdx];
}

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient)
    :   TabInstance(view, TabType::PatientSummary, patient), 
        view(view->summaryView()),
         tab_presenter(tabPresenter),
        m_currentFrameIdx{ -1 },
        statusTimeFrame(DbPatientSummary::getFrames(patient->rowid))
{
    auto perioStatuses = DbPerio::getAllPerioStatuses(patient->rowid);

    statusTimeFrame.reserve(statusTimeFrame.size() + perioStatuses.size());

    //inserting perioTimeFrames into their corresponding locations:

    for (int i = 0; i < perioStatuses.size(); i++) {
        TimeFrame t{
            TimeFrameType::Perio,
            perioStatuses[i].rowid,
            {},
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

    if (statusTimeFrame.size()) m_currentFrameIdx = 0;

}

void PatientSummaryPresenter::openCurrentDocument()
{
    auto frame = currentFrame();

    if (!tab_presenter || !frame) return;

    if (frame->LPK != User::doctor().LPK)
    {
        ModalDialogBuilder::showMessage(
            u8"Документът принадлежи на друг доктор и не може да бъде отворен"
        );
        return;
    }

    switch (frame->type) {
        case TimeFrameType::Perio:
        {
            auto row = RowInstance(TabType::PerioList);
            row.rowID = frame->rowid;
            row.patientRowId = patient->rowid;
            tab_presenter->open(row, true);
            break;
        }
        default:
        {
            auto row = RowInstance(TabType::AmbList);
            row.rowID = frame->rowid;
            row.patientRowId = patient->rowid;
            tab_presenter->open(row, true);
            break;
        }
    }
}

void PatientSummaryPresenter::setCurrentFrame(int index)
{
    m_currentFrameIdx = index;

    toothSelected(m_selectedTooth);

    auto frame = currentFrame();

    if (!frame) return;

    view->setTeeth(ToothHintCreator::getTeethHint(statusTimeFrame[m_currentFrameIdx].teeth));
    view->setPerioData(PerioWithDisabled(frame->perioData));
 
    view->setDocumentLabel(
        frame->getFrameName(),
        frame->date.toString(true),
        User::getNameFromLPK(frame->LPK)
    );

    switch (frame->type)
    {
    case TimeFrameType::InitialAmb:
        view->setInitialAmbList();
        break;
    case::TimeFrameType::Procedures:
        view->setProcedures(frame->procedures);
        break;
    case::TimeFrameType::Perio:
        auto stat = PerioStatistic(frame->perioData, patient->getAge(frame->date));
        view->setPerioStatistic(stat);
        break;
      
    }

}

void PatientSummaryPresenter::openPatientDialog()
{
    PatientDialogPresenter p{ *patient };

    auto patient = p.open();

    if (!patient.has_value()) return;

    *this->patient = patient.value();

    view->setPatient( *this->patient.get() );
}

void PatientSummaryPresenter::openAllergiesDialog()
{
        AllergiesDialogPresenter p(*patient.get());

        auto data = p.openDialog();

        if (!data.has_value()) return;

        auto& d = data.value();

        patient->allergies = d.allergies;
        patient->currentDiseases = d.current;
        patient->pastDiseases = d.past;

        view->setPatient(*this->patient.get());
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


    view->setPatient(*patient.get());
    view->setTimeFrameCount(statusTimeFrame.size());
    view->setTickPosition(m_currentFrameIdx);  
    view->setSelectedTooth(m_selectedTooth);

    setCurrentFrame(m_currentFrameIdx);

    _tabView->showSummaryView();
}

void PatientSummaryPresenter::toothSelected(int toothIdx)
{
    m_selectedTooth = toothIdx;

    auto frame = currentFrame();

    if (toothIdx == -1 || !frame) {
        //set no toothInfo in the view
        view->setToothInfo({});
        return;
    }

    view->setToothInfo(frame->teeth.at(toothIdx).getToothInfo());
    
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
