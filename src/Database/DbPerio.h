#pragma once
#include "AbstractORM.h"
#include "Model/Tooth/ToothContainer.h"
#include "Model/Date.h"

class DbPerio : public AbstractORM
{
public:
	ToothContainer getStatus(const std::string& patientID, const Date& perioDate);
};

