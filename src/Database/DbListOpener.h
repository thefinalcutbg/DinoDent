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
	std::vector<PrescriptionRow> getPrescriptionRows(const Date& from, const Date& to);
	void deleteRecord(TabType type, long long rowid);
};

