#pragma once
#include <vector>

#include "Recipient.h"
#include "NhifInvoiceData.h"
#include "Issuer.h"
#include "BusinessOperation.h"
#include "FinancialEnums.h"
class TiXmlDocument;

struct Practice;
struct Doctor;

struct MainDocument //only in case of debit or credit note
{
	long long number{1};
	Date date{Date::currentDate()};
};


struct AggregatedAmounts
{

	PaymentType paymentType{PaymentType::Cash};

	double total_amount {0};
	double payment_amount {0};
	Date taxEventDate;

	void calculate(const BusinessOperations& operations);
};



struct Invoice
{
	Invoice() {};
	Invoice(const Recipient& r);
	Invoice(const TiXmlDocument& monthNotif, const Practice& practice, const Doctor& doctor);
	Invoice(const Patient& p);

	std::optional<MainDocument> mainDocument() const;

	void setMainDocumentData(long long num, Date date);

	long long number{ 0 };
	long long rowId{0};
	FinancialDocType type {FinancialDocType::Invoice};
	std::string name{ "Фактура" }; //the title of the pdf invoice
	Date date; //input by user !!!!!!!!!!!!!!!
	std::optional<NhifInvoiceData> nhifData; //from monthNotif
	Recipient recipient;
	Issuer issuer() const;
	BusinessOperations businessOperations;
	AggregatedAmounts aggragated_amounts;

	std::string getInvoiceNumber() const;

	void removeOperation(int idx);
	void addOperation(const BusinessOperation& op);
	void editOperation(const BusinessOperation& op, int idx);
	std::string getFileName();
	
	//Invoice& operator= (const Invoice& other);

private:
	MainDocument m_mainDocument; //for credit and debit note

};