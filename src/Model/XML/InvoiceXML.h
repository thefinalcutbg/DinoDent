#pragma once
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <optional>
#include "Model/Date.h"

class TiXmlDocument;
struct User;
struct Practice;

struct Recipient
{
	std::string code;
	std::string name;
	std::string address;
	std::string bulstat;
};

struct MainDocument //only in case of debit or credit note
{
	const std::string number;
	const Date date;
};


struct SelfInsured
{
	//trqbwa i dannite za liceto!
	const char self_insured;
	const std::string_view self_insured_declaration;
};

struct Company
{
	const std::string_view legal_form;
};

typedef std::variant<SelfInsured, Company> IssuerType;

struct Issuer
{
	Issuer(const Practice& practice);

	const IssuerType issuer_type;
	const std::string company_name;
	const std::string address_by_contract;
	const bool registration_by_VAT;
	const std::string grounds_for_not_charging_VAT;
	const std::string issuer_bulstat;
	const std::string contract_no;
	const Date contract_date;
	const std::string rhi_nhif_no;

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

struct AggregatedAmount
{

};

struct Invoice
{
	Invoice(const TiXmlDocument& monthNotifRoot, const User& user);

	const std::string fin_document_type_code; //INVOICE, DT_NOTIF, CT_NOTIF

	std::string fin_document_no; //leading zeroes! input by user !!!!!!!!!!!

	const std::string fin_document_month_no; //monthly_notification_num

	std::string fin_document_date; //input by user !!!!!!!!!!!!!!!

	std::optional<MainDocument> mainDocument; //for credit and debit note

	const Recipient& recipient;
	const Issuer issuer;

	std::string health_insurance_fund_type_code;
	const int activityTypeCode;
	
	const Date date_from;
	const Date date_to;
	/*
	const std::string payment_type;
	const std::string total_amount;
	const std::string vat_rate;
	const char original;
	const std::string taxEventDate;
	*/
	std::vector<BusinessOperation> businessOperations;



};