#pragma once
#include "AbstractORM.h"
#include "Model/Tooth/ToothContainer.h"
#include "Model/Date.h"
#include "Model/PerioStatus.h"

class DbPerio : public AbstractORM
{
public:
	ToothContainer getStatus(const std::string& patientID, const Date& date);

	PerioStatus getPerioStatus(const std::string& patientID, Date date);
	void insertPerioStatus(PerioStatus& perioStatus, const std::string& patientID);
	void updatePerioStatus(const PerioStatus& perioStatus);
};
