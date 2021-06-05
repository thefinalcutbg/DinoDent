#pragma once
#include "Model/CheckState.h"
#include "Model/Tooth/Tooth.h"

constexpr int ActionCount = 25;

enum class StatusAction {
	Temporary,
	Obturation,
	Caries,
	Pulpitis,
	ApicalLesion,
	EndoTreatment,
	Post,
	Root, Fracture,
	Extraction,
	Periodontitis,
	Mobility1,
	Mobility2,
	Mobility3,
	Crown,
	Bridge,
	Implant,
	Dsn,
	MO,
	DO,
	MOD,
	removeO,
	removeC,
	removeAll,
	removeBridge
};

enum class SurfaceType { obturation, caries };

enum class ButtonPos { left, up, right, down, center, side };

