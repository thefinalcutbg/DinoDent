#pragma once
#include <unordered_set>
#include "Model/Financial/Invoice.h"
#include <tuple>

struct NzokFinancialDetails {
	std::string rowId;
	int num;
	Date date;
};

namespace DbInvoice
{
	int getNewInvoiceNumber();
	std::unordered_set<int>getExistingNumbers();
	std::string insertInvoice(const Invoice& invoice);
	void updateInvoice(const Invoice& invoice);
	std::optional<NzokFinancialDetails> getDetailsIfAlreadyExist(int monthNotifNumber);
	Invoice getInvoice(const std::string& rowId);
	
}