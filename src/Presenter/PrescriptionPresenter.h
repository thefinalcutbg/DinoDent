#pragma once
#include "TabInstance.h"

#include "Model/Prescription/Prescription.h"
#include "Network/HIS/EPrescription.h"
#include "Presenter/PatientInfoPresenter.h"

class TabView;
class PrescriptionView;

class PrescriptionPresenter : public TabInstance
{
	PrescriptionView* view;

	Prescription m_prescription;

	PatientInfoPresenter patient_info;

	EPrescription::Issue issue_prescription;
	EPrescription::Cancel cancel_prescription;
	EPrescription::FetchDispense check_status_service;
	EPrescription::eRxFetch fetch_service;

	void sendPrescriptionToHis();
	void cancelPrescription();

	void prepareDerivedForSwitch() override {
		patient_info.setCurrent(false);
	}

public:
	PrescriptionPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowId = 0);

	void nrnButtonClicked();
	void checkStatus();
	void addPressed();
	void editPressed(int idx);
	void deletePressed(int idx);
	void eRxPressed();
	void addTemplate();
	

	void setFemaleProperties(bool pregnancy, bool breastfeeding);

	void supplementsChanged(const std::string& s);
	void dispensationChanged(const Dispensation& d);
	void dateChanged(const Date& date);
	// Inherited via TabInstance
	virtual void setDataToView() override;

	virtual long long rowID() const override;

	bool save() override;
	void print() override;
	bool isNew() override;
	TabName getTabName() override;
	void pdfPrint() override final;
};

