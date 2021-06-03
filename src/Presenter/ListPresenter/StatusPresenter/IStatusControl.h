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

class IStatusControl
{
public:
	virtual void changeStatus(StatusAction code) = 0;
	virtual void changeStatus(Surface surface, SurfaceType type) = 0;
};

