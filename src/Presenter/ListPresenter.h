#pragma once

#include <vector>
#include <memory>
#include "View/Interfaces/IListView.h"
#include "Model/Referrals/RefNum.h"

#include "Presenter/TabInstance.h"
#include "SurfacePanelPresenter.h"
#include "CheckModel.h"

#include "Network/PIS/DentalActivitiesService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"
#include "Network/HIS/EDental.h"
#include "Network/HIS/EReferral.h"
#include "Network/HIS/EMedicalNotice.h"
#include "Network/HIS/EHospitalization.h"
#include "Network/NRA/NssiPentionService.h"
#include "Presenter/PatientInfoPresenter.h"

typedef std::vector<int> SelectedTeethIdx;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;
    PatientInfoPresenter patient_info;

    SelectedTeethIdx m_selectedIndexes;

    CheckModel m_checkModel;
    CheckModel m_dsnCheckModel;

    IListView* view;

    bool firstFocus{ true };

    std::shared_ptr<DentalActivitiesService> dentalActService;
    std::shared_ptr<NraStatusService> nraStatusServ;
    std::shared_ptr<NssiPentionService> nssiService;
    std::shared_ptr<DiagnosisService> nhifDiagnosisServ;
    std::shared_ptr<EDental::Open> eDentalOpenService;
    std::shared_ptr<EDental::Cancel> eDentalCancelService;
    std::shared_ptr<EDental::Augment> eDentalAugmentService;
    std::shared_ptr<EDental::GetStatusAndProcedures> eDentalGetStatusAndProceduresService;
    std::shared_ptr<EReferral::Issue> eReferralIssueService;
    std::shared_ptr<EReferral::Cancel> eReferralCancelService;
    std::shared_ptr<EMedicalNotice::Issue> eMedicalNoticeIssue;
    std::shared_ptr<EMedicalNotice::Cancel> eMedicalNoticeCancel;
    std::shared_ptr<EHospitalization::Fetch> eHospitalizationFetch;

    void prepareDerivedForSwitch() override {
        patient_info.setCurrent(false);
    }

    int generateAmbListNumber();

    bool isValid();
    void refreshProcedureView();
    void statusChanged();
    void setHisButtonToView();
    void makeEdited() override;

    //call when adding or removing procedures and referrals
    void dynamicNhifConversion();

public:

    AmbList m_ambList;

    ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, long long rowId = 0);

    long long rowID() const override;
    bool save() override;
    bool isNew() override;
    void print() override;
    TabName getTabName() override;
    void setDataToView() override;

    void setAmbDateTime(const std::string& datetime);
    void ambNumChanged(long long value);

    void checkPention();
    void addFinancialReceipt();

    void setToothStatus(Dental::StatusType t, int code, bool supernumeral = false);
    void setOther(int code);

    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

    void historyRequested();

    void openDetails(int toothIdx);
    void openDetails();

    void addProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);
    void moveProcedure(int from, int to);

    void checkHospitalization();

    void addMedicalNotice();
    void editMedicalNotice(int index);
    void removeMedicalNotice(int index);
    void sendMedicalNoticeToHis(int index);

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

    ~ListPresenter();
};

