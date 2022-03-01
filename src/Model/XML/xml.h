#pragma once
#include <string>
#include <variant>
#include "Model/Date.h"
struct Invoice;


struct ReportResult
{
	bool success{ false };
	std::string message;
};

namespace XML
{
	ReportResult saveXMLreport(int month, int year, const std::string& path);
	void saveXMLinvoice(const Invoice& invioice, const std::string& path);
	
};
