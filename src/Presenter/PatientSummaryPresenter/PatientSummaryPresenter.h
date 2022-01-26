#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include <memory>

class PatientSummaryView;
class Patient;

class PatientSummaryPresenter final : public TabInstance
{
	IPatientSummaryView* view;

	std::shared_ptr<Patient> patient;

public:
	PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient);

	bool save() override { return true; };
	bool saveAs() override { return true; };
	bool isNew() override { return false; };


	void print() override;
	void setCurrent() override;

	virtual std::string getTabName() override;
};

