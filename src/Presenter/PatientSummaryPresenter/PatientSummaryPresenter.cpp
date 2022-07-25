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
#include "Model/FreeFunctions.h"

TimeFrame* PatientSummaryPresenter::currentFrame()
{
    if (state.currentIdx < 0 ||
        state.currentIdx >= statusTimeFrame.size()) 
    {
        return nullptr;
    }

    return &statusTimeFrame[state.currentIdx];
}

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient)
    :   TabInstance(view, TabType::PatientSummary, patient), 
        view(view->summaryView()),
         tab_presenter(tabPresenter),

        statusTimeFrame(DbPatientSummary::getFrames(patient->rowid))
{
    auto perioStatuses = DbPerio::getAllPerioStatuses(patient->rowid);

    state.showPerioGraph = !perioStatuses.empty();

    statusTimeFrame.reserve(statusTimeFrame.size() + perioStatuses.size());

    //inserting perioTimeFrames into their corresponding locations:

    for (int i = 0; i < perioStatuses.size(); i++) {

        TimeFrame perioFrame{
            TimeFrameType::Perio,
            perioStatuses[i].rowid,
            {},
            perioStatuses[i].LPK,
            perioStatuses[i].date,
            ToothContainer{},
            {},
            perioStatuses[i]

        };

        int insertAtIdx = 0;
        
        for (int y = 0; y < statusTimeFrame.size(); y++) {

            if (statusTimeFrame[y].date > perioFrame.date)
            {
                //the position at which the perio status frame should be inserted:
                insertAtIdx = y;
                break;
            }
        }
        
        statusTimeFrame.insert(statusTimeFrame.begin() + insertAtIdx, perioFrame);

        //getting status from the previous timeframe
        if (insertAtIdx) {
            statusTimeFrame[insertAtIdx].teeth = statusTimeFrame[insertAtIdx - 1].teeth;
        }
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

    state.frameCount = statusTimeFrame.size();
    if (state.frameCount) state.currentIdx = 0;

    //getting the price period:

    bool firstFound{ false };

    for (auto& frame : statusTimeFrame)
        for (auto& p : frame.procedures)
        {
            if (!firstFound) {
                state.from = p.date;
                firstFound = true;
            }

            state.to = p.date;
        }

}

void PatientSummaryPresenter::openCurrentDocument()
{
    auto frame = currentFrame();

    if (!tab_presenter || !frame) return;

    if (frame->LPK != User::doctor().LPK)
    {
        ModalDialogBuilder::showMessage(
            u8"Документът принадлежи на друг доктор и не може да бъде отворен за корекция"
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

void PatientSummaryPresenter::pricePeriodChanged(const Date& from, const Date& to)
{
    double price = 0;
    double nzokPrice = 0;

    state.from = from;
    state.to = to;

    for (auto& frame : statusTimeFrame)
        for (auto& p : frame.procedures)        
        {
            if (p.date < from || p.LPK != User::doctor().LPK) continue;
            if (p.date > to) goto finish;

            price += p.price;

            if (p.nzok) {
                nzokPrice +=
                MasterNZOK::instance().getNZOKPrice(
                    p.code,
                    p.date,
                    User::doctor().specialty,
                    patient->isAdult(p.date),
                    false);
            }
        }


finish:

    
    view->setPrice(PriceInfoStr{ price, nzokPrice });

}

void PatientSummaryPresenter::setCurrentFrame(int index)
{
    state.currentIdx = index;

    auto frame = currentFrame();

    if (!frame) return;

    toothSelected(state.selectedTooth);

    view->setTeeth(ToothHintCreator::getTeethHint(frame->teeth));
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

    refreshTabName();
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


void PatientSummaryPresenter::setDataToView()
{
    view->setPresenter(this);

    view->setPatient(*patient.get());

    view->setUiState(state);
}

void PatientSummaryPresenter::toothSelected(int toothIdx)
{
    state.selectedTooth = toothIdx;

    auto frame = currentFrame();

    if (toothIdx == -1 || !frame)  return;


    view->setToothInfo(ToothInfoStr{
            frame->teeth[toothIdx].toothName(),
            frame->teeth.at(toothIdx).getToothInfo(),
            patient->teethNotes[toothIdx]
        });
    
}

void PatientSummaryPresenter::teethViewButtonClicked(bool showBuccal)
{
    state.buccalSelected = showBuccal;
}

void PatientSummaryPresenter::perioCheckBoxClicked(bool checked)
{
    state.showPerioGraph = checked;
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
