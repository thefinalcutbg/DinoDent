#pragma once

#include "Model/TimeFrame.h"

namespace DbPatientSummary
{

	std::vector<TimeFrame> getFrames(const std::string& patientID);
};

