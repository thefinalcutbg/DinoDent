#pragma once

#include "Model/Tooth/ToothContainer.h"
#include "Model/Date.h"
#include "Model/PerioStatus.h"

namespace DbPerio
{

	ToothContainer getStatus(long long patientRowId, const Date& date);

	PerioStatus getPerioStatus(long long patientRowId, Date date);
	PerioStatus getPerioStatus(long long rowid);
	std::vector<PerioStatus> getAllPerioStatuses(long long patientRowId);
	long long insertPerioStatus(const PerioStatus& perioStatus, long long patientRowId);
	void updatePerioStatus(const PerioStatus& perioStatus);
};

