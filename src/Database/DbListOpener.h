#pragma once
#include <vector>
#include "Model/TableRows.h"
#include "Model/Patient.h"

namespace DbListOpener
{

	std::vector<PatientRow> getPatientRows();
	std::vector<AmbRow> getAmbRows(const Date& from, const Date& to);
	std::vector<PerioRow> getPerioRows(const Date& from, const Date& to);
	std::vector<FinancialRow> getFinancialRows(const Date& from, const Date& to);
	void deleteRecord(const std::string& tableName, const std::string& id);
};

