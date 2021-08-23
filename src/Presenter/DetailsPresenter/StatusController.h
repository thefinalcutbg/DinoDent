#pragma once
#include "View/DetailsView/IDetailedStatusView.h"
#include "Model/Tooth/Status.h"
#include <QDebug>

class DetailedStatusController
{
protected:
	IDetailedStatusView& view;

public:

	DetailedStatusController(IDetailedStatusView& view) : view(view) {}

	virtual void applyChange() = 0;

	virtual ~DetailedStatusController() { };
};

class PathologyControl : public DetailedStatusController
{
	Pathology& status;
public:

	PathologyControl(IDetailedStatusView& view, Pathology& status) : 
	DetailedStatusController(view), 
	status(status)
	{
		view.setData(status.data);
	}

	void applyChange() override {
		status.data.diagnosis_index = view.getPathologyData();
	}


};

class DentistMadeControl : public DetailedStatusController
{
	DentistMade& status;
public:
	DentistMadeControl(IDetailedStatusView& view, DentistMade& status) :
	DetailedStatusController(view),
	status(status)
	{
		view.setData(status.getDentistData());
	}

	void applyChange() override {
		status.setUser(view.getDentistData());
	}

};

class ObturationControl : public DetailedStatusController
{
	Obturation& status;

public:
	ObturationControl(IDetailedStatusView& view, Obturation& status) :
		DetailedStatusController(view),
		status(status)

	{
		view.setData(status.data);
		view.setData(status.getDentistData());
	}

	void applyChange() override { 
	status.data = view.getObturationData();
	status.setUser(view.getDentistData());
	}
};


class CrownControl : public DetailedStatusController
{
	Crown& status;
public:
	CrownControl(IDetailedStatusView& view, Crown& status) :
		DetailedStatusController(view),
		status(status)

	{
		view.setData(status.data);
		view.setData(status.getDentistData());
	}

	void applyChange() override { 
	status.data = view.getCrownData();
	status.setUser(view.getDentistData());
	}
};

class ImplantControl : public DetailedStatusController
{
	Implant& status;

public:
	ImplantControl(IDetailedStatusView& view, Implant& status) :
		DetailedStatusController(view),
		status(status)
	{
		view.setData(status.data);
		view.setData(status.getDentistData());
	}

	void applyChange() override {
		status.data = view.getImplantData();
		status.setUser(view.getDentistData());
	}

};