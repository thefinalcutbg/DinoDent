#pragma once
#include "TabInstance.h"
#include "Presenter/PatientInfoPresenter.h"
#include "Model/Dental/TreatmentPlan.h"

class TreatmentPlanView;

class TreatmentPlanPresenter : public TabInstance
{

    PatientInfoPresenter patient_info;
    TreatmentPlanView* view = nullptr;

    //stage and procedure
    std::pair<int, int> m_selection = {-1, -1};

    std::vector<long long> m_deleted_procedures;

    TreatmentPlan m_treatmentPlan;

    void removeStage();
    void removeProcedure();
    bool invalidSelection();
    void setTeethToView();
    void setCompletedProcedures();

public:
    TreatmentPlanPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowId = 0);

protected:
    void setDataToView();
    void prepareDerivedForSwitch();

public:
    void patientDataChanged();
    bool isNew();
    TabName getTabName();
    bool save();
    void print();
    void pdfPrint();
    long long rowID() const;

    void addStage();
    void addProcedure(const std::vector<int>& teeth_idx);
    void addConclusion();
    void editPressed();
    void removePressed();
    void dateChanged(const Date& date);
    void selectionChanged(const std::pair<int, int>& stageProcedurePair);
    void priceEditRequested();
    void nameEditRequested();
    void setCompleted(bool completed);
};
