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

public:

	PerioPresenter(ITabView* view, std::shared_ptr<Patient> patient);


	// Inherited via TabInstance
	virtual bool save() override;
	virtual bool saveAs() override;
	virtual bool isNew() override;
	virtual void print() override;
	virtual void setCurrent() override;
	virtual void prepareSwitch() override;
	virtual std::string getTabName() override;
};

