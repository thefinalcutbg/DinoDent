#pragma once
#include <memory>

#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/Patient.h"
#include "View/PerioView/IPerioView.h"
#include "Model/PerioStatistic.h"
#include "Model/PerioStatus.h"
#include "Model/Tooth/ToothContainer.h"

class PerioPresenter : public TabInstance
{

	IPerioView* view;

	ShowTeeth m_teethShow{ false };

	void refreshMeasurment(int index);

	int getRecession(int surfaceIndex);

public:

	PerioStatus m_perioStatus;
	ToothContainer m_toothStatus;

	PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient);
	PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient, long long rowId);
	void toothButtonClicked(int tooth);

	void pdChanged(int index, int value);
	void calChanged(int index, int value);
	void gmChanged(int index, int value);
	void bopChanged(int index, bool checked);
	void attachChanged(int index, int value);
	void FMBSChanged(int index, bool value);
	void FMPSChanged(int index, bool value);
	void teethViewChanged(ShowTeeth t);
	void furcationChanged(int index, int a, int b, int c);

	void smokeClicked(int value);
	void boneLossChanged(int value);
	void systemicChanged(bool enabled);
	void restorationChanged(bool enabled);

	void openPatientDialog();
	void openAllergiesDialog();

	// Inherited via TabInstance
	long long rowID() const override;
	virtual bool save() override;
	virtual bool saveAs() override;
	virtual bool isNew() override;
	virtual void print() override;
	virtual void setCurrent() override;
	virtual void prepareSwitch() override;
	virtual TabName getTabName() override;

	~PerioPresenter();
};

