#pragma once
#include "Model/User/UserManager.h"
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
	std::string& LPK;
	UserManager& m_manager;
	bool permissionToWrite{ false };

public:
	DentistMadeControl(IDetailedStatusView& view, DentistMade& status) :
	DetailedStatusController(view),
	LPK(status.LPK),
	m_manager(UserManager::instance())
	{

		DentistData data;
		data.dentistName = m_manager.getDoctorName(LPK);
		data.isChecked = (!LPK.empty());
		data.isEnabled = (LPK.empty() || m_manager.isCurrentUser(LPK));

		permissionToWrite = data.isEnabled;

		view.setData(data);
	}

	void applyChange() override {

		if (!permissionToWrite) return; 

		auto isChecked = view.getDentistData();

		if (isChecked)
			LPK = m_manager.currentUser().LPK;
		else LPK.clear();
	}

};

class ObturationControl : public DetailedStatusController
{
	Obturation& status;

	DentistMadeControl m_dentistCtrl;

public:
	ObturationControl(IDetailedStatusView& view, Obturation& status) :
		DetailedStatusController(view),
		status(status),
		m_dentistCtrl(view, status)

	{
		view.setData(status.data);
	}

	void applyChange() override { 
	status.data = view.getObturationData();
	m_dentistCtrl.applyChange();
	}
};


class CrownControl : public DetailedStatusController
{
	Crown& status;

	DentistMadeControl m_dentistCtrl;
public:
	CrownControl(IDetailedStatusView& view, Crown& status) :
		DetailedStatusController(view),
		status(status),
		m_dentistCtrl(view, status)

	{
		view.setData(status.data);
	}

	void applyChange() override { 
	status.data = view.getCrownData();
	m_dentistCtrl.applyChange();
	}
};

class ImplantControl : public DetailedStatusController
{
	Implant& status;
	DentistMadeControl m_dentistCtrl;

public:
	ImplantControl(IDetailedStatusView& view, Implant& status) :
		DetailedStatusController(view),
		status(status),
		m_dentistCtrl(view, status)
	{
		view.setData(status.data);
	}

	void applyChange() override {
		status.data = view.getImplantData();
		m_dentistCtrl.applyChange();
	}

};

class NotesControl : public DetailedStatusController
{
	std::string& notes;

public: 
	NotesControl(IDetailedStatusView& view, std::string& notes) :
		DetailedStatusController(view),
		notes(notes)
{
	view.setData(notes);
}
	void applyChange() override {
		notes = view.getNotes();
	}
};