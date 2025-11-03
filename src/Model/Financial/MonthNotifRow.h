#pragma once
#include <string>
#include "Model/Date.h"
#include "Model/Financial/FinancialEnums.h"

struct MonthNotifRow {

	std::string hash;
	Date date;
	FinancialDocType type;
	std::string rziCode;
	std::string uin;
	bool is_claimed{ false };

};
