#pragma once
#include <string>
#include <variant>
#include "Model/Date.h"
#include <unordered_map>
struct Invoice;

#include "Model/Dental/AmbList.h"
#include "Model/Patient.h"

struct ReportResult
{
	std::optional<std::string> report;
	std::string message;
};

namespace XML
{
	//ReportResult getXMLReport(int month, int year);
	std::string getReport(const std::vector<AmbList>& lists, const std::unordered_map<long long, Patient>& patients);
	std::string getInvoice(const Invoice& invoice);

	
};
