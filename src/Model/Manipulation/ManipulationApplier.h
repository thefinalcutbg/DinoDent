#pragma once
#include "Manipulation.h"
#include "../Tooth/ToothController/BridgeController.h"
#include "../Tooth/ToothController/GeneralStatusControler.h"
#include "../Tooth/ToothController/SurfaceController.h"

class ManipulationApplier
{
	ObturationController obtur_ctrl;
	CariesController caries_ctrl;
	GeneralStatusController status_ctrl;
	BridgeController bridge_ctrl;

public:
	void applyManipulations(const std::vector<Manipulation>& m, std::array<Tooth, 32>& teeth, const std::string& LPK);
};

