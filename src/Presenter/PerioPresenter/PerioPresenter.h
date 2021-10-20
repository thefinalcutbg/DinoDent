#pragma once
#include <memory>

#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/Patient.h"
#include "View/PerioView/IPerioView.h"
#include "Model/PerioStatus.h"
#include "Database/DbPerio.h"

class PerioPresenter : public TabInstance
{
	DbPerio m_db;
	Date perioDate;
	std::shared_ptr<Patient> patient;
	IPerioView* view;
	ToothContainer m_toothStatus;
	PerioStatus m_perioStatus;

	void refreshMeasurment(int index);

	int getRecession(int surfaceIndex);

public:

	PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient);
	void toothButtonClicked(int tooth);

	void pdChanged(int index, int value);
	void calChanged(int index, int value);
	void gmChanged(int index, int value);
	void bopChanged(int index, bool checked);
	void attachChanged(int index, int value);
	void FMBSChanged(int index, bool value);
	void FMPSChanged(int index, bool value);

	// Inherited via TabInstance
	virtual bool save() override;
	virtual bool saveAs() override;
	virtual bool isNew() override;
	virtual void print() override;
	virtual void setCurrent() override;
	virtual void prepareSwitch() override;
	virtual std::string getTabName() override;
};

