#pragma once
#include "View/DetailsView/SubWidgets/IDetailedStatusView.h"
#include "Model/Tooth/Status.h"
class StatusController
{
protected:
	IDetailedStatusView* view{ nullptr };

public:



	void setView(IDetailedStatusView* view) { this->view = view; }
	virtual void commitChange() = 0;

	virtual ~StatusController() {};
};

class PathologyControl : public StatusController
{
	Pathology* status;
public:
	void setStatus(Pathology* status)
	{
		this->status = status;
		view->setData(status->data);
	}

	void commitChange() override { 
	status->data.diagnosis_index = view->getPathologyData();
	}
};

class DentistMadeControl : public StatusController
{
	DentistMade* status;
public:

	void setStatus(DentistMade* status)
	{
		this->status = status;
		view->setData(status->getDentistData());
	}

	void commitChange() override {
		status->setUser(view->getDentistData());
	}

};

class ObturationControl : public StatusController
{
	Obturation* status;

public:
	void setStatus(Obturation* status)
	{
		this->status = status;
		view->setData(status->data);
		view->setData(status->getDentistData());
	}

	void commitChange() override { 
	status->data = view->getObturationData();
	status->setUser(view->getDentistData());
	view->setData(status->data);

	}
};


class CrownControl : public StatusController
{
	Crown* status;
public:

	void setStatus(Crown* status)
	{
		this->status = status;
		view->setData(status->data);
		view->setData(status->getDentistData());
	}

	void commitChange() override { 
	status->data = view->getCrownData();
	status->setUser(view->getDentistData());
	}
};

class ImplantControl : public StatusController
{
	Implant* status;

public:
	void setStatus(Implant* status)
	{
		this->status = status;
		view->setData(status->data);
		view->setData(status->getDentistData());

	}

	void commitChange() override {
		status->data = view->getImplantData();
		status->setUser(view->getDentistData());
	}

};