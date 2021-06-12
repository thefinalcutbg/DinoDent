#pragma once

#include <vector>


#include "View/ListView/IListView.h"


#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "StatusPresenter/StatusPresenter.h"
#include "ProcedurePresenter/ProcedurePresenter.h"
#include <memory>
#include "Editor.h"

class ListInstance;

class ListPresenter :
    public AllergiesDialogRequestor,
    public Editor
{

    StatusPresenter status_presenter;
    ProcedurePresenter procedure_presenter;

    IListView* view;

    AllergiesDialogPresenter* allergiesDialog;

    AmbList* ambList;
    std::weak_ptr<Patient> patient;


public:
    ListPresenter();

    void setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter);
    void setData(ListInstance* listInstance);
    void setView(IListView* view);
    void attachEditObserver(EditObserver* observer) override;
  
    void openPatientDialog();
    void openAllergiesDialog();

    // Inherited via PatientDialogRequestor
    void setAllergies(Allergies allergies) override;

};

