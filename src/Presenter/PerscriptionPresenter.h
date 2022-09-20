#pragma once
#include "TabInstance.h"
#include "View/Interfaces/IPerscriptionView.h"
#include "Model/Perscription/Perscription.h"

class PerscriptionPresenter : public TabInstance
{
	IPerscriptionView* view;

	Perscription m_perscription;

public:
	PerscriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

	void addPressed();
	void editPressed(int idx);
	void deletePressed(int idx);

	void supplementsChanged(const std::string& s);
	
	void dispensationChanged(const Dispensation& d);

	// Inherited via TabInstance
	virtual void setDataToView() override;

	virtual long long rowID() const override;

	virtual bool save() override;

	virtual void print() override;

	virtual bool isNew() override;

	virtual TabName getTabName() override;

};

