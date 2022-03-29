#pragma once
#include <vector>

#include "Recipient.h"
#include "NzokInvoiceData.h"
#include "Issuer.h"
#include "BusinessOperation.h"

class TiXmlDocument;
struct User;
struct Practice;


struct MainDocument //only in case of debit or credit note
{
	const int number;
	const Date date;
};

enum class PaymentType { Cash, Bank };

struct AggregatedAmounts
{

	PaymentType paymentType{PaymentType::Cash};

	double total_amount {0};
	double payment_amount {0};
	Date taxEventDate;

	void calculate(const BusinessOperations& operations);
};

enum class FinancialDocType { Invoice, Debit, Credit};

struct Invoice
{
	Invoice() {};
	Invoice(const TiXmlDocument& monthNotif, const User& user);
	Invoice(const Patient& p, const User& user);

	int number{ 0 };

	long long rowId{0};

	FinancialDocType type {FinancialDocType::Invoice};
	
	std::string name{ u8"Фактура" }; //the title of the pdf invoice

	Date date; //input by user !!!!!!!!!!!!!!!

	std::optional<MainDocument> mainDocument; //for credit and debit note

	std::optional<NZOKInvoiceData> nzokData; //from monthNotif

	Recipient recipient;
	Issuer issuer;
	
	BusinessOperations businessOperations;

	AggregatedAmounts aggragated_amounts;

	std::string getInvoiceNumber() const;

	void removeOperation(int idx);
	void addOperation(const BusinessOperation& op);
	void editOperation(const BusinessOperation& op, int idx);
	std::string getFileName();
	


};