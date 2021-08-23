#pragma once
#include "AbstractORM.h"
#include <vector>
#include <string>

#include "Model/Date.h"

struct ProcedureEntry
{
	int tooth{ -1 };
	Date date;
	std::string procedureName;
	std::string diagnosis;
	std::string doctor;
};

typedef std::vector<ProcedureEntry> ProcedureHistory;

class DbTooth : public AbstractORM
{
public:
	ProcedureHistory getHistory(std::string patientId, int tooth);
};

