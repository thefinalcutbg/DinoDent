#pragma once

#include "Model/Tooth/ToothContainer.h"
#include "Model/Date.h"
#include "Model/PerioStatus.h"

namespace DbPerio
{

	ToothContainer getStatus(const std::string& patientID, const Date& date);

	PerioStatus getPerioStatus(const std::string& patientID, Date date);
	PerioStatus getPerioStatus(const std::string& perioID);
	void insertPerioStatus(PerioStatus& perioStatus, const std::string& patientID);
	void updatePerioStatus(const PerioStatus& perioStatus);
};

