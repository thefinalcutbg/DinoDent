#pragma once

#include <vector>
#include <memory>
#include "Model/Referrals/RefNum.h"
#include "Database/DbAmbList.h"

#include "Presenter/TabInstance.h"
#include "SurfacePanelPresenter.h"
#include "CheckModel.h"

#include "Network/PIS/DentalActivitiesService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"
#include "Network/HIS/EDental.h"
#include "Network/HIS/EReferral.h"
#include "Network/HIS/EMedicalNotice.h"
#include "Network/NRA/NssiPentionService.h"
#include "Presenter/PatientInfoPresenter.h"
#include "Network/BulkRequester.h"

typedef std::vector<int> SelectedTeethIdx;

class TabView;
class ListView;

class ListPresenter : public TabInstance
{
    SurfacePanelPresenter surf_presenter;
    PatientInfoPresenter patient_info;

    SelectedTeethIdx m_selectedIndexes;

    CheckModel m_checkModel;
    CheckModel m_dsnCheckModel;

    ListView* view;

    bool firstFocus{ true };

	BulkRequester bulkRequester;
   
    NssiPentionService nssiService;
 
    EDental::Open eDentalOpenService;
    EDental::Cancel eDentalCancelService;
    EDental::Augment eDentalAugmentService;
	EDental::Fetch eDentalFetchService;

    EReferral::Issue eReferralIssueService;
    EReferral::Cancel eReferralCancelService;

    EMedicalNotice::Issue eMedicalNoticeIssue;
    EMedicalNotice::Cancel eMedicalNoticeCancel;
    

    void prepareDerivedForSwitch() override {
        patient_info.setCurrent(false);
    }

    bool isValid();
    void refreshProcedureView();
    void statusChanged();
    void setHisButtonToView();
    void makeEdited() override;
    void makeEditedOnTimeChange();
    void printPrv(bool toPdf);
    void fetchListProcedures(const std::string& nrn);
	void handleBulkRequestResult(const BulkRequester::Result& result);

    //call when adding or removing procedures and referrals
    void dynamicNhifConversion();
    //when changing the patient data from the current list
    void patientDataChanged() override;

    void setSignature(const std::vector<unsigned char> sig_bitmap, const std::string sig_data);
public:

    AmbList m_amblist;

    ListPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowId = 0);

    long long rowID() const override;
    bool save() override;
    bool isNew() override;
    void print() override;
    void pdfPrint() override;
    TabName getTabName() override;
    void setDataToView() override;

    void setAmbDateTime(const std::string& datetime);
	void setTreatmentEndTime(const std::string& datetime);

    void checkPention();
    void addFinancialReceipt();

    void setToothStatus(Dental::StatusType t, int code, bool supernumeral = false);
    void setOther(int code);

    void setSelectedTeeth(const std::vector<int>& SelectedIndexes);
    
    void historyRequested();

    void openDetails(int toothIdx);
    void openDetails();

    void addProcedure();
    void addPlannedProcedure();
    void editProcedure(int index);
    void deleteProcedure(int index);
    void moveProcedure(int from, int to);

    void showAppliedStatus();
    void showSignature();

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
    void createTreatmentPlan();
    void printDeclarations();

    void hisButtonPressed();
	void cancelHisAmbList();
	void sendToHis(bool patientIsSigner = true);


    ~ListPresenter();
};

