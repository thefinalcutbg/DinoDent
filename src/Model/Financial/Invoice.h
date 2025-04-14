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


struct Invoice
{
	Invoice() {};
	Invoice(const Recipient& r);
	Invoice(const TiXmlDocument& monthNotif, const std::string& claimedHash, const Practice& practice, const Doctor& doctor);
	Invoice(const Patient& p);

	std::optional<MainDocument> mainDocument() const;

	long long number{ 0 };
	long long rowId{0};

	FinancialDocType type {FinancialDocType::Invoice};

	Date date; //input by user !!!!!!!!!!!!!!!
	std::optional<NhifInvoiceData> nhifData; //from monthNotif

	Recipient recipient;
	Issuer issuer() const;
	BusinessOperations businessOperations;
	PaymentType paymentType{ PaymentType::Cash };
	Date taxEventDate;

	std::string title() const; //the title of the pdf invoice
	std::string getFileName() const;
	std::string nhifDocumentTypeCode() const;
	std::string getInvoiceNumber() const;

	void removeOperation(int idx);
	void addOperation(const BusinessOperation& op);
	void editOperation(const BusinessOperation& op, int idx);
    bool isVAT {false};
	double amount() const;
    double VATamount() const;
	void setMainDocumentData(long long num, Date date);

private:
	MainDocument m_mainDocument; //for credit and debit note

};
