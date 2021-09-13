#pragma once

#include <vector>
#include <memory>
#include "View/ListView/IListView.h"

#include "Database/DbAmbList.h"
#include "Model/AmbList.h"

#include "Presenter/TabPresenter/TabInstance.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "CheckState.h"


typedef std::vector<int> SelectedTeethIdx;
typedef std::vector<Tooth*> SelectedTeeth;
typedef std::vector<Procedure> Procedures;

class ListPresenter : public TabInstance
{
    DbAmbList db;

    SurfacePanelPresenter surf_presenter;

    SelectedTeethIdx m_selectedIndexes;
    SelectedTeeth m_selectedTeeth;
    int m_selectedProcedure;

    CheckModel m_checkModel;

    IListView* view;

    bool isValid();
    void refreshProcedureView();
    void statusChanged();
    void hasBeenEdited();
public:

    AmbList m_ambList;
    std::shared_ptr<Patient> patient;

    ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient);
    ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, const std::string& ambListId);

    void chargeChanged(int index);


    virtual bool save() override;
    virtual bool saveAs() override;
    virtual bool close() override;
    virtual void print() override;
    virtual std::string getTabName() override;
    void setCurrent() override;

    void openPatientDialog();
    void openAllergiesDialog();

    void setCaries(int surface);
    void setObturation(int surface);
    void setMainStatus(int code);
    void setOther(int code);

    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

    void openDetails(int toothIdx);
    void openDetails();

    void addToProcedureList(const std::vector<Procedure>& new_mList);


    void addProcedure();
    void editProcedure();
    void deleteProcedure(int index);
    void ambDateChanged(Date date);
    void setSelectedProcedure(int index);
    void setUnfavourable(bool unfav);

    ~ListPresenter();
};

