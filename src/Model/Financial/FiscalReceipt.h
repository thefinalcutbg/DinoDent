#pragma once
#include <string>
#include <vector>

struct FiscalReceipt
{
	std::string datetime;
	long long amblistRowid;
	std::string fiscal_memory;
	std::string receipt_num;
	std::string amblistNumber; //returned only from DbFiscalReceipt::getReceipts
	std::string patientId;	//returned only from DbFiscalReceipt::getReceipts
};


struct FiscalReport
{
	std::string filename;
	std::string dateFrom;
	std::string dateTo;
	std::vector<FiscalReceipt> receipts;
};