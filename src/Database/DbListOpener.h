#pragma once
#include <vector>

#include "AbstractORM.h"
#include "Model/TableRows.h"
#include "Model/Patient.h"

class DbListOpener : public AbstractORM
{
public:
	std::vector<PatientRow> getPatientRows();
	std::vector<AmbRow> getAmbRows(const Date& from, const Date& to);
	std::vector<PerioRow> getPerioRows(const Date& from, const Date& to);

	void deleteRecord(const std::string& tableName, const std::string& id);
};

