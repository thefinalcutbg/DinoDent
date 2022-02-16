#pragma once
#include <string>
#include "Model/Date.h"
struct Invoice;

namespace XML
{
	void saveXMLreport(int month, int year, const std::string& path);
	void saveXMLinvoice(const Invoice& invioice, const std::string& path);
	
};
