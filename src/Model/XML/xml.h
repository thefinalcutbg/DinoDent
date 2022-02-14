#pragma once
#include <string>
#include "Model/Date.h"

namespace XML
{
	void saveXMLfile(int month, int year, std::string path);
	void makeInvoice(std::string month_notif_path);
	
};
