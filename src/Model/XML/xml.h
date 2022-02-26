#pragma once
#include <string>
#include <optional>
#include "Model/Date.h"
struct Invoice;

typedef std::optional<std::string> Errors;

namespace XML
{
	Errors saveXMLreport(int month, int year, const std::string& path);
	void saveXMLinvoice(const Invoice& invioice, const std::string& path);
	
};
