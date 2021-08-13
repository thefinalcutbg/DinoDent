#pragma once

#include "View/ListView/IStatusView.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "../Editor.h"
#include "CheckState.h"
#include <vector>

class ProcedurePresenter;

class StatusPresenter : public Editor
{
	IStatusView* view;

	SurfacePanelPresenter surf_presenter;
	ProcedurePresenter* procedure_presenter;

	ToothContainer* teeth;
	std::vector<Tooth*>* selectedTeeth;

	CheckModel m_checkModel{};

	std::vector<int> getSelectedIndexes();

	void statusChanged();
public:
	
	void setData(ToothContainer& teeth, std::vector<Tooth*>& selectedTeeth);
	void setView(IStatusView* view);
	void setProcedurePresenter(ProcedurePresenter* p);

	void setCaries(int surface);
	void setObturation(int surface);
	void setMainStatus(int code);
	void setOther(int code);

	void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

	void openDetails(int tooth);
	void openDetails();


};

