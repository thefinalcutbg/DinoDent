#pragma once
#include "Model/User.h"
#include "View/Interfaces/IDetailedStatusView.h"
#include "Model/Dental/Status.h"
#include "Model/Dental/SurfStatus.h"

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
		view.setData(status);
	}

	void applyChange() override {
		status.setDiagnosisIdx(view.getDiagnosisIndex());
	}


};

class DentistMadeControl : public DetailedStatusController
{
	std::string& LPK;

	bool permissionToWrite{ false };

	std::string getMadeByLabel(const std::string& statusLPK) const
	{
		if (statusLPK.empty()) {
			return User::doctor().getFullName();
		}

		return User::getNameFromLPK(statusLPK);
	}

public:
	DentistMadeControl(IDetailedStatusView& view, DentistMade& status) :
	DetailedStatusController(view),
	LPK(status.LPK)
	{

		DentistData data;
		data.dentistName = getMadeByLabel(LPK);
		data.isChecked = (!LPK.empty());
		data.isEnabled = (LPK.empty() || User::isCurrentUser(LPK));

		permissionToWrite = data.isEnabled;

		view.setData(data);
	}

	void applyChange() override {

		if (!permissionToWrite) return; 

		auto isChecked = view.getDentistData();

		if (isChecked)
			LPK = User::doctor().LPK;
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

class ObturationBulk : public DetailedStatusController
{
	SurfaceStatus<SurfaceChild<Obturation>>& status;

public:
	ObturationBulk(IDetailedStatusView& view, SurfaceStatus<SurfaceChild<Obturation>>& status) :
		DetailedStatusController(view),
		status(status)
	{
		std::string LPK;
		bool madeByCurrentUser{ false };

		std::vector<int> existingSurfaces;

		for (int i = 0; i < surfaceCount; i++)
		{
			if (!status.exists(i))
				continue;

			existingSurfaces.push_back(i);
		}

		if (existingSurfaces.size())
		{
			LPK = status[existingSurfaces[0]].LPK;


			for (auto i : existingSurfaces)
			{
				if (LPK != status[i].LPK) {
					LPK = "";
					break;
				}
			}

		}

		DentistData dentistData;

		dentistData.dentistName = LPK.empty() ?
			User::doctor().getFullName()
			:
			User::getNameFromLPK(LPK);

		dentistData.isChecked = (!LPK.empty());
		dentistData.isEnabled = (LPK.empty() || User::isCurrentUser(LPK));

		view.setData(dentistData);
		view.setData(ObturationData{});

	}

	void applyChange() override
	{
		auto statusResult = view.getObturationData();
		auto doctorResult = view.getDentistData();

		for (int i = 0; i < surfaceCount; i++)
		{

			if (!status.exists(i)) continue;

			if (statusResult.color.getIndex()) status[i].data.color = VitaColor(statusResult.color);

			status[i].data.material = statusResult.material;

			if (status[i].LPK == User::doctor().LPK || status[i].LPK.empty())
				status[i].LPK = doctorResult ? User::doctor().LPK : "";

		}
	};

};


class CariesBulk : public DetailedStatusController
{
	SurfaceStatus<SurfaceChild<Caries>>& status;
	
	Caries output_data; //output_data is needed for evaluation whether user has changed the index

public:
	CariesBulk(IDetailedStatusView& view, SurfaceStatus<SurfaceChild<Caries>>& status) :
		DetailedStatusController(view),
		status(status)
	{
		Date uninitializedDate(1,1,1);

		output_data.date_diagnosed = uninitializedDate;

		for (int i = 0; i < surfaceCount; i++)
		{
			if(!status.exists(i))
				continue;

			output_data.setDiagnosisIdx(std::max(status[i].getDiagnosisIdx(), output_data.getDiagnosisIdx()));
			output_data.date_diagnosed = std::max(status[i].date_diagnosed, output_data.date_diagnosed);
		}

		if (output_data.date_diagnosed == uninitializedDate)
			output_data.date_diagnosed = Date::currentDate();

		view.setData(output_data);

	}

	void applyChange() override
	{
		auto statusResult = view.getDiagnosisIndex();

		for (int i = 0; i < surfaceCount; i++)
		{
			if (statusResult != output_data.getDiagnosisIdx()) //checking if the index has been changed at all
				status[i].setDiagnosisIdx(statusResult);
		}
	};

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

class PostControl : public DetailedStatusController
{
	Post& status;

	DentistMadeControl m_dentistCtrl;
public:
	PostControl(IDetailedStatusView& view, Post& status) :
		DetailedStatusController(view),
		status(status),
		m_dentistCtrl(view, status)

	{
		view.setData(status.data);
	}

	void applyChange() override {
		status.data = view.getPostData();
		m_dentistCtrl.applyChange();
	}
};

class FiberSplintControl : public DetailedStatusController
{
	FiberSplint& status;

	DentistMadeControl m_dentistCtrl;
public:
	FiberSplintControl(IDetailedStatusView& view, FiberSplint& status) :
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