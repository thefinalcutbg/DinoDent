#pragma once
#include <string>

struct FiscalReceipt
{
	std::string datetime;
	long long amblistRowid;
	std::string fiscal_memory;
	std::string receipt_num;
	std::string amblistNumber; //returned only from DbFiscalReceipt::getReceipts
};
