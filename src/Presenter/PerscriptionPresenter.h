#pragma once
#include "TabInstance.h"
#include "View/Interfaces/IPerscriptionView.h"

class PerscriptionPresenter : public TabInstance
{
	IPerscriptionView* view;
public:
	PerscriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId = 0);

	void addPressed();


	// Inherited via TabInstance
	virtual void setDataToView() override;

	virtual long long rowID() const override;

	virtual bool save() override;

	virtual void print() override;

	virtual bool isNew() override;

	virtual TabName getTabName() override;

};

