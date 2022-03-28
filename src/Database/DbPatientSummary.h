#pragma once

#include "Model/TimeFrame.h"

namespace DbPatientSummary
{

	std::vector<TimeFrame> getFrames(long long patientRowId);
};

