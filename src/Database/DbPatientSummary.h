#pragma once
#include "AbstractORM.h"
#include "Model/TimeFrame.h"

class DbPatientSummary : public AbstractORM
{
public:
	std::vector<TimeFrame> getFrames(const std::string& patientID);
};

