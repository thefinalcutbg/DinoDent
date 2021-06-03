#pragma once

#include <vector>

#include "Model/ListInstance.h"
#include "View/ListView/IListView.h"

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "StatusPresenter/StatusPresenter.h"
#include "ProcedurePresenter/ProcedurePresenter.h"

#include "Editor.h"


class ListPresenter :
    public PatientDialogRequestor,
    public AllergiesDialogRequestor,
    public Editor
{

    StatusPresenter status_presenter;
    ProcedurePresenter procedure_presenter;

    IListView* view;

    PatientDialogPresenter *patientDialog;
    AllergiesDialogPresenter* allergiesDialog;

    AmbList* ambList;
    Patient* patient;


public:
    ListPresenter();

    void setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter);

    void setPatientDialog(PatientDialogPresenter* presenter);

    void setData(ListInstance* listInstance);
    void setView(IListView* view);
    void attachEditObserver(EditObserver* observer) override;
  
    void openPatientDialog();
    void openAllergiesDialog();

    // Inherited via PatientDialogRequestor
    void setPatient(Patient patient) override;
    void setAllergies(Allergies allergies) override;

};

