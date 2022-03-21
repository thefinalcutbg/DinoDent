#pragma once
#include <vector>

#include "Recipient.h"
#include "NzokInvoiceData.h"
#include "Issuer.h"

class TiXmlDocument;
struct User;
struct Practice;


struct MainDocument //only in case of debit or credit note
{
	const std::string number;
	const Date date;
};


struct BusinessOperation
{
	const std::string activity_code;
	const std::string activity_name;
	const static inline std::string measure_code{ "BR" };
	const double unit_price;
	const int quantity;
	const double value_price;
};

struct AggregatedAmounts
{
	const char* payment_type{ "B" };
	double total_amount {0};
	double payment_amount {0};
	//const std::string vat_rate;
	const char* original {"Y"};
	Date taxEventDate;
};

enum class FinancialDocType { Invoice, Debit, Credit};

struct Invoice
{
	Invoice(const TiXmlDocument& monthNotifRoot, const User& user);

	std::string rowId;

	FinancialDocType type;

	std::string name; //the title of the pdf invoice
	std::string number; //leading zeroes! input by user !!!!!!!!!!!
	Date date; //input by user !!!!!!!!!!!!!!!

	std::optional<MainDocument> mainDocument; //for credit and debit note

	std::optional<NZOKInvoiceData> nzokData; //from monthNotif

	Recipient recipient;
	const Issuer issuer;
	
	std::vector<BusinessOperation> businessOperations;

	AggregatedAmounts aggragated_amounts;

};