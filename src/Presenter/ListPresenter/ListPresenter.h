#pragma once

#include <vector>

#include "Model/ListInstance.h"
#include "Model/Manipulation/getManipulationTamplates.h"
#include "View/ListView/IListView.h"

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "StatusPresenter/StatusPresenter.h"

#include "Editor.h"


class ListPresenter :
    public PatientDialogRequestor,
    public AllergiesDialogRequestor,
    public Editor
{

    StatusPresenter status_presenter;

    IListView* view;

    PatientDialogPresenter *patientDialog;
    AllergiesDialogPresenter* allergiesDialog;

    AmbList* ambList;
    Patient* patient;

    void addToManipulationList(const std::vector<Manipulation> &m);
    void refreshManipulationView(const std::vector<Manipulation>& m);

public:
    ListPresenter();

    void setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter);

    void setPatientDialog(PatientDialogPresenter* presenter);

    void setData(ListInstance* listInstance);
    void setView(IListView* view);
    void attachEditObserver(EditObserver* observer) override;

    void setUnfavourable(bool unfav);
  
    void openPatientDialog();
    void openAllergiesDialog();

    void addProcedure();
    void deleteProcedure(int index);

    void manipulationSelected(int index);
    // Inherited via PatientDialogRequestor
    void setPatient(Patient patient) override;
    void setAllergies(Allergies allergies) override;

};

