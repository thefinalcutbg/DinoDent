#pragma once
#include "CheckModelCreator.h"
#include "StatusControl.h"
#include "View/ListView/IStatusView.h"
#include "ToothHintCreator.h"
#include "Model/Tooth/ToothController/BridgeController.h"
#include "SurfacePanel/SurfacePanelPresenter.h"
#include "../Editor.h"
#include <vector>

class ProcedurePresenter;

class StatusPresenter : public Editor
{
	CheckModelCreator checkCreator;
	StatusControl statusControl;
	ToothHintCreator paint_hint_generator;
	BridgeController bridgeController;
	SurfacePanelPresenter surf_presenter;
	IStatusView* view;
	ProcedurePresenter* procedure_presenter;

	std::array<Tooth, 32>* teeth;
	std::vector<Tooth*>* selectedTeeth;

	std::vector<int> getSelectedIndexes();

	void statusChanged();
public:
	
	void setData(std::array<Tooth, 32>& teeth, std::vector<Tooth*>& selectedTeeth);
	void setView(IStatusView* view);
	void setProcedurePresenter(ProcedurePresenter* p);

	void changeStatus(Surface surface, SurfaceType type);
	void changeStatus(StatusAction status);
	void setSelectedTeeth(const std::vector<int>& SelectedIndexes);

};

