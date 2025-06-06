﻿#include <array>

#include "PatientSummaryPresenter.h"
#include "View/Interfaces/ITabView.h"
#include "View/Interfaces/IPatientSummaryView.h"
#include "Model/Patient.h"
#include "Database/DbPatientSummary.h"

#include "Presenter/TabPresenter.h"

#include "Model/TableRows.h"
#include "Model/User.h"
#include "Model/Dental/PerioStatistic.h"

TimeFrame* PatientSummaryPresenter::currentFrame()
{
    if (state.currentIdx < 0 ||
        state.currentIdx >= statusTimeFrame.size()) 
    {
        return nullptr;
    }

    return &statusTimeFrame[state.currentIdx];
}

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient)
    :
    TabInstance(view, TabType::PatientSummary, patient),
    view(view->summaryView()),
    patient_presenter(view->summaryView()->patientTile(), patient),
    statusTimeFrame(DbPatientSummary::getFrames(patient->rowid))
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

    if (!frame) return;

    if (frame->LPK != User::doctor().LPK)
    {
        ModalDialogBuilder::showMessage(
            "Документът принадлежи на друг лекар и не може да бъде отворен за корекция"
        );
        return;
    }

    switch (frame->type) {
        case TimeFrameType::Perio:
        {
            auto row = RowInstance(TabType::PerioStatus);
            row.rowID = frame->rowid;
            row.patientRowId = patient->rowid;
            TabPresenter::get().open(row, true);
            break;
        }
        default:
        {
            auto row = RowInstance(TabType::AmbList);
            row.rowID = frame->rowid;
            row.patientRowId = patient->rowid;
            TabPresenter::get().open(row, true);
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

    std::array<ToothPaintHint, 32> hints;

    for (int i = 0; i < 32; i++) hints[i] = ToothPaintHint(frame->teeth[i]);

    view->setTeeth(hints);
    view->setPerioData(PerioWithDisabled(frame->perioData));
 
    view->setDocumentLabel(
        frame->getFrameName(),
        frame->date.toBgStandard(true),
        User::getNameFromLPKorRHIF(frame->LPK)
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
        PerioStatistic stat = PerioStatistic(frame->perioData, patient->getAge(frame->date));
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
    TabPresenter::get().openList(*patient);
}

void PatientSummaryPresenter::newPerio()
{
    TabPresenter::get().openPerio(*patient);
}

void PatientSummaryPresenter::newPrescription()
{
    TabPresenter::get().openPerscription(*patient);
}

void PatientSummaryPresenter::newInvoice()
{
    TabPresenter::get().openInvoice(patient->rowid);
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
