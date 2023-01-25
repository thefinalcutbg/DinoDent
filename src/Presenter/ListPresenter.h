#pragma once

#include <vector>
#include <memory>
#include "View/Interfaces/IListView.h"
#include "Model/Referrals/RefNum.h"
#include "Database/DbAmbList.h"

#include "Presenter/TabInstance.h"
#include "SurfacePanelPresenter.h"
#include "CheckState.h"

#include "Network/PIS/DentalActivitiesService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"
#include "Presenter/PatientInfoPresenter.h"

typedef std::vector<int> SelectedTeethIdx;
typedef std::vector<Tooth*> SelectedTeeth;

class TabPresenter;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;
    PatientInfoPresenter patient_info;

    SelectedTeethIdx m_selectedIndexes;
    SelectedTeeth m_selectedTeeth;

    CheckModel m_checkModel;

    IListView* view;

    bool m_openHistoryDialogOnReply { false };

    TabPresenter* tabPresenter;

    DentalActivitiesService dentalActService;
    NraStatusService nraStatusServ;
    DiagnosisService nhifDiagnosisServ;

    void prepareDerivedForSwitch() override {
        patient_info.setCurrent(false);
    }

    int generateAmbListNumber();
    void requestPisActivities();
    bool isValid();
    void refreshProcedureView();
    void statusChanged();
    void refreshPrices();

    //call when adding or removing procedures and referrals
    void dynamicNhifConversion();

public:

    AmbList m_ambList;

    ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

    void chargeChanged(int index);

    long long rowID() const override;
    bool save() override;
    bool isNew() override;
    void print() override;
    TabName getTabName() override;
    void setDataToView() override;

    void ambNumChanged(long long value);
    void setCaries(int surface);
    void setObturation(int surface);
    void setMainStatus(int code);
    void setMobility(int degree);
    void setOther(int code);
    
    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);


    void setPISActivities(
        const std::optional<std::vector<Procedure>>& pisProcedures,
        const std::vector<std::string>& payment_status
    );

    void openPisHistory();
    void openDetails(int toothIdx);
    void openDetails();

    void addProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);

    void addReferral(ReferralType type);
    void editReferral(int index);
    void removeReferral(int index);
    void printReferral(int index);
    void sendReferralToHis(int index);

    void setNhifData(const NhifSheetData& data);

    void createInvoice();
    void createPerioMeasurment();


    ~ListPresenter();
};

