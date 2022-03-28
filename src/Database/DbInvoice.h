#pragma once
#include <unordered_set>
#include "Model/Financial/Invoice.h"
#include <tuple>

struct NzokFinancialDetails {
	long long rowId;
	int num;
	Date date;
};

namespace DbInvoice
{
	int getNewInvoiceNumber();
	std::unordered_set<int>getExistingNumbers();
	long long insertInvoice(const Invoice& invoice);
	void updateInvoice(const Invoice& invoice);
	std::optional<NzokFinancialDetails> getDetailsIfAlreadyExist(int monthNotifNumber);
	Invoice getInvoice(long long rowId);
	
}