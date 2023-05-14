#include <array>

#include "PatientSummaryPresenter.h"
#include "View/Interfaces/ITabView.h"
#include "View/Interfaces/IPatientSummaryView.h"
#include "ToothHintCreator.h"
#include "Model/Patient.h"
#include "Database/DbPatientSummary.h"
#include "Database/DbPerio.h"
#include "Presenter/PatientDialogPresenter.h"
#include "Model/Dental/PerioStatistic.h"
#include "Presenter/TabPresenter.h"
#include "Model/TableRows.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Model/Dental/NhifProcedures.h"
#include "qdebug.h"

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
        statusTimeFrame(DbPatientSummary::getFrames(patient->rowid)),
        patient_presenter(view->summaryView()->patientTile(), patient)
{

    state.frameCount = statusTimeFrame.size();
    if (state.frameCount) state.currentIdx = 0;

    for (auto& f : statusTimeFrame)
    {
        if (f.type == TimeFrameType::Perio)
        {
            state.showPerioGraph = true;
        }
    }
}

void PatientSummaryPresenter::openCurrentDocument()
{
    auto frame = currentFrame();

    if (!tab_presenter || !frame) return;

    if (frame->LPK != User::doctor().LPK)
    {
        ModalDialogBuilder::showMessage(
            "Документът принадлежи на друг доктор и не може да бъде отворен за корекция"
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
    state.currentIdx = index;

    auto frame = currentFrame();

    if (!frame) return;

    toothSelected(state.selectedTooth);

    view->setTeeth(ToothHintCreator::getTeethHint(frame->teeth));
    view->setPerioData(PerioWithDisabled(frame->perioData));
 
    view->setDocumentLabel(
        frame->getFrameName(),
        frame->date.toBgStandard(true),
        User::getNameFromLPK(frame->LPK)
    );

    patient_presenter.setDate(frame->date);

    switch (frame->type)
    {
    case TimeFrameType::InitialAmb:
        view->setProcedures({});
        view->setInitialAmbList();
        break;
    case::TimeFrameType::Procedures:
        view->setProcedures(frame->procedures);
        break;
    case::TimeFrameType::Perio:
        view->setProcedures({});
        auto stat = PerioStatistic(frame->perioData, patient->getAge(frame->date));
        view->setPerioStatistic(stat);
        break;
      
    }

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

    patient_presenter.setCurrent(true);

    view->setUiState(state);
}

void PatientSummaryPresenter::newAmbSheet()
{
    tab_presenter->openList(*patient);
}

void PatientSummaryPresenter::newPerio()
{
    tab_presenter->openPerio(*patient);
}

void PatientSummaryPresenter::newPrescription()
{
    tab_presenter->openPerscription(*patient);
}

void PatientSummaryPresenter::newInvoice()
{
    tab_presenter->openInvoice(patient->rowid);
}

void PatientSummaryPresenter::toothSelected(int toothIdx)
{
    state.selectedTooth = toothIdx;

    auto frame = currentFrame();

    if (toothIdx == -1 || !frame) {
        view->hideToothInfo();
        return;
    }


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
        "Пациент",
        this->patient->firstLastName()
    };
}

PatientSummaryPresenter::~PatientSummaryPresenter()
{
    if (view != nullptr) view->setPresenter(nullptr);
}
