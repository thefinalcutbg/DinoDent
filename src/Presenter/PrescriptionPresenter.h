#pragma once
#include "TabInstance.h"
#include "View/Interfaces/IPrescriptionView.h"
#include "Model/Prescription/Prescription.h"
#include "Network/HIS/EPrescription.h"
#include "Presenter/PatientInfoPresenter.h"
class PrescriptionPresenter : public TabInstance
{
	IPrescriptionView* view;

	Prescription m_prescription;

	PatientInfoPresenter patient_presenter;

	EPrescription::Issue issue_prescription;
	EPrescription::Cancel cancel_prescription;

	void sendPrescriptionToHis();
	void cancelPrescription();

public:
	PrescriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

	void nrnButtonClicked();

	void addPressed();
	void editPressed(int idx);
	void deletePressed(int idx);

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

};

