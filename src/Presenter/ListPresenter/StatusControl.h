#pragma once
#include <array>
#include <vector>

#include "Model/CheckState.h"
#include "Model/Tooth/ToothController/GeneralStatusControler.h"
#include "Model/Tooth/ToothController/SurfaceController.h"
#include "IStatusControl.h"


class StatusControl : public CheckModelReciever, public IStatusControl
{
	std::vector<Tooth*> selectedTeeth;

	CheckModel checkModel;

	GeneralStatusController general_controller;
	ObturationController obturation_controller;
	CariesController caries_controller;

	template<int size>
	void changeAnyStatus(int statusIndex, std::array<CheckState, size>& arrayModel, ToothController& controller);

public:
	StatusControl();

	void setSelectedTeeth(const std::vector<Tooth*>& selectedTeeth);
	void setCheckModel(const CheckModel& checkModel);

	void changeStatus(StatusAction code);
	void changeStatus(Surface surface, SurfaceType type);

};


