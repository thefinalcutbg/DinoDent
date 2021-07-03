#pragma once
#include "Procedure.h"
#include "../Tooth/ToothController/BridgeController.h"
#include "../Tooth/ToothController/GeneralStatusControler.h"
#include "../Tooth/ToothController/SurfaceController.h"

class ProcedureApplier
{
	ObturationController obtur_ctrl;
	CariesController caries_ctrl;
	GeneralStatusController status_ctrl;
	BridgeController bridge_ctrl;

public:
	void applyProcedures(const std::vector<Procedure>& m, std::array<Tooth, 32>& teeth, const std::string& LPK);
};

