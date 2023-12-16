#pragma once
#include <vector>
#include "Model/TableRows.h"
#include "Model/Patient.h"
#include "Model/PlainTable.h"


namespace DbBrowser
{
	std::pair<std::vector<RowInstance>, PlainTable> getData(TabType type, const Date& from, const Date& to);
	void deleteRecord(TabType type, long long rowid);
	std::pair<std::vector<RowInstance>, PlainTable> getPatientDocuments(long long patientRowid);
};

