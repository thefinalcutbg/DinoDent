#pragma once

#include <vector>
#include <memory>
#include "View/ListView/IListView.h"

#include "Database/DbAmbList.h"

#include "Presenter/TabPresenter/TabInstance.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "CheckState.h"

#include "Network/PIS/DentalActivitiesService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"

typedef std::vector<int> SelectedTeethIdx;
typedef std::vector<Tooth*> SelectedTeeth;


class TabPresenter;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;

    SelectedTeethIdx m_selectedIndexes;
    SelectedTeeth m_selectedTeeth;

    bool m_showCurrentStatus{ false };
    bool m_showInsuranceDialog{ false };

    CheckModel m_checkModel;

    IListView* view;

    bool m_openHistoryDialogOnReply { false };

    TabPresenter* tabPresenter;

    DentalActivitiesService dentalActService;
    NraStatusService nraStatusServ;
    DiagnosisService nhifDiagnosisServ;

    void requestPisActivities();
    bool isValid();
    void refreshProcedureView();
    void statusChanged();

public:

    AmbList m_ambList;

    ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

    void chargeChanged(int index);

    long long rowID() const override;
    bool save() override;
    bool saveAs() override;
    bool isNew() override;
    void print() override;
    TabName getTabName() override;
    void setDataToView() override;

    void openPatientDialog();
    void openAllergiesDialog();

    void setCaries(int surface);
    void setObturation(int surface);
    void setMainStatus(int code);
    void setOther(int code);
    
    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);


    void setPISActivities(const std::optional<Procedures>& pisProcedures);
    void setInsuranceStatus(const std::optional<InsuranceStatus>& status_result);

    void openPisHistory();
    void checkHealthInsurance(bool showDialog);
    void checkDiagnosisNhif();
    void openDetails(int toothIdx);
    void openDetails();

    void addToProcedureList(const std::vector<Procedure>& new_mList);

    void addProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);
    void setfullCoverage(bool unfav);
    void createInvoice();
    void createPerioMeasurment();
    void showCurrentStatus(bool show);

    ~ListPresenter();
};

