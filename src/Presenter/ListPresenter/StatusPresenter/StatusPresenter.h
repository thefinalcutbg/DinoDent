#pragma once
#include "CheckModelCreator.h"
#include "StatusControl.h"
#include "View/ListView/IStatusView.h"
#include "ToothHintCreator.h"
#include "Model/Tooth/ToothController/BridgeController.h"
#include "../Editor.h"
#include <vector>


class StatusPresenter : public IStatusControl, public Editor
{
	CheckModelCreator checkCreator;
	StatusControl statusControl;
	ToothHintCreator paint_hint_generator;
	BridgeController bridgeController;

	IStatusView* view;

	std::array<Tooth, 32>* teeth;
	std::vector<int>* selectedIndexes;

	

	void statusChanged();
public:
	
	void setData(std::array<Tooth, 32>& teeth, std::vector<int>& selectedIndexes);
	void setView(IStatusView* view);
	void changeStatus(Surface surface, SurfaceType type);
	void changeStatus(StatusAction status);
	void setSelectedTeeth(const std::vector<int>& SelectedIndexes);


	std::vector<Tooth*> getSelectedTeethPointers();
};

