#pragma once
#include "Model/Financial/FiscalReceipt.h"
#include <vector>

namespace DbFiscalReceipt {

	std::string getFiscalMemory();

	std::vector<FiscalReceipt> getReceipts(int month, int year);

	bool alreadyExists(long long ambRowid, const std::string& date);

	void saveReceipt(const FiscalReceipt& r);
	void deleteReceipt(long long ambRowid, const std::string& receipt_timestamp);
}