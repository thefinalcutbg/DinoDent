#pragma once
#include "CheckModelCreator.h"
#include "StatusControl.h"
#include "View/ListView/IStatusView.h"

#include "Model/Tooth/ToothController/BridgeController.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "../Editor.h"
#include <vector>

class ProcedurePresenter;

class StatusPresenter : public Editor
{
	CheckModelCreator checkCreator;
	StatusControl statusControl;
	BridgeController bridgeController;
	SurfacePanelPresenter surf_presenter;
	IStatusView* view;
	ProcedurePresenter* procedure_presenter;

	ToothContainer* teeth;
	std::vector<Tooth*>* selectedTeeth;

	std::vector<int> getSelectedIndexes();

	void statusChanged();
public:
	
	void setData(ToothContainer& teeth, std::vector<Tooth*>& selectedTeeth);
	void setView(IStatusView* view);
	void setProcedurePresenter(ProcedurePresenter* p);

	void changeStatus(Surface surface, SurfaceType type);
	void changeStatus(StatusAction status);
	void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

	void openDetails(int tooth);
	void openDetails();


};

