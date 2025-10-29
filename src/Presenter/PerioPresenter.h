#pragma once
#include <memory>

#include "Presenter/TabInstance.h"
#include "Presenter/PatientInfoPresenter.h"
#include "Model/Patient.h"
#include "Model/Dental/PerioStatistic.h"
#include "Model/Dental/PerioStatus.h"
#include "Model/Dental/ToothContainer.h"

class TabView;
class PerioView;

class PerioPresenter : public TabInstance
{

	PerioView* view;

	bool m_showLowerTeeth{ false };

	void refreshMeasurment(int index);

	int getRecession(int surfaceIndex);
	PatientInfoPresenter patient_info;

	void prepareDerivedForSwitch() override {
		patient_info.setCurrent(false);
	}

public:

	PerioStatus m_perioStatus;
	ToothContainer m_toothStatus;

	PerioPresenter(TabView* view, std::shared_ptr<Patient> patient, long long rowId = 0);
	void toothButtonClicked(int tooth);

	void pdChanged(int index, int value);
	void calChanged(int index, int value);
	void gmChanged(int index, int value);
	void bopChanged(int index, bool checked);
	void attachChanged(int index, int value);
	void FMBSChanged(int index, bool value);
	void FMPSChanged(int index, bool value);
	void teethViewChanged(bool showLower);
	void furcationChanged(int index, int a, int b, int c);

	void smokeClicked(int value);
	void boneLossChanged(int value);
	void systemicChanged(bool enabled);
	void restorationChanged(bool enabled);

	void dateChanged(const Date& date);

	// Inherited via TabInstance
	long long rowID() const override;
	bool save() override;
	bool isNew() override;
	void print() override;
	void setDataToView() override;
	void prepareSwitch() override;
	TabName getTabName() override;

	~PerioPresenter();
};

