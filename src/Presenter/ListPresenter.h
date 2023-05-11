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
#include "Network/HIS/EDental.h"
#include "Network/HIS/EReferral.h"
#include "Presenter/PatientInfoPresenter.h"

typedef std::vector<int> SelectedTeethIdx;
//typedef std::vector<Tooth*> SelectedTeeth;

class TabPresenter;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;
    PatientInfoPresenter patient_info;

    SelectedTeethIdx m_selectedIndexes;

    CheckModel m_checkModel;
    CheckModel m_dsnCheckModel;

    IListView* view;

    bool firstFocus{ true };

    TabPresenter* tabPresenter;

    DentalActivitiesService dentalActService;
    NraStatusService nraStatusServ;
    DiagnosisService nhifDiagnosisServ;
    EDental::Open eDentalOpenService;
    EDental::Cancel eDentalCancelService;
    EDental::GetStatus eDentalGetStatus;
    EDental::Augment eDentalAugmentService;
    EDental::GetProcedures eDentalGetProcedures;
    EReferral::Issue eReferralIssueService;
    EReferral::Cancel eReferralCancelService;

    std::vector<Procedure> getToothHistory(int toothIdx);

    void prepareDerivedForSwitch() override {
        patient_info.setCurrent(false);
    }

    int generateAmbListNumber();
    
    bool isValid();
    void refreshProcedureView();
    void statusChanged();
    void refreshPrices();
    void setHisButtonToView();
    void makeEdited() override;

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

    void setAmbDateTime(const std::string& datetime);
    void ambNumChanged(long long value);


    void setToothStatus(StatusType t, int code);
    void setDsnStatus(StatusType t, int code);
    void setOther(int code);

    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);
    
    void requestPisActivities(bool clickedByUser);
    void requestHisActivities(bool clickedByUser);

    void openDetails(int toothIdx);
    void openDetails();

    void addProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);
    void moveProcedure(int from, int to);

    void addReferral(ReferralType type);
    void editReferral(int index);
    void removeReferral(int index);
    void printReferral(int index);
    void sendReferralToHis(int index);

    void setNhifData(const NhifSheetData& data);

    void createInvoice();
    void createPerioMeasurment();
    void createPrescription();
    
    void hisButtonPressed();
    void getStatusPressed();

    ~ListPresenter();
};

