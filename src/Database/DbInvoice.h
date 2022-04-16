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
	//returns rowid of existing invoice for this monthly notification:
	long long invoiceAlreadyExists(int monthNotifNumber);
	std::optional<Date> getMainDocDate(long long invoiceNumber, const std::string& recipientId);
	std::optional<MainDocument> getMainDocument(const std::string& recipient_id);
	Invoice getInvoice(long long rowId);
	
}