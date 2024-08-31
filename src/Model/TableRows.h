#pragma once

#include "Presenter/TabInstance.h"

struct RowInstance
{
	const TabType type;
	long long rowID{0};
	long long patientRowId{0};
	RowInstance(TabType t) : type(t) {}
};