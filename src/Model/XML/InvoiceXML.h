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


struct PersonInfo
{
	const std::string identifier;
	const std::string first_name;
	const std::string second_name;
	const std::string last_name;

};

struct SelfInsured
{
	const std::string self_insured_declaration;
	const PersonInfo person_info;
};

struct Company
{
	const std::string legal_form;
};

typedef std::variant<Company, SelfInsured> IssuerType;

struct Issuer
{
	Issuer(const User& user);

	const IssuerType type;
	const std::string company_name;
	const std::string address_by_contract;
	const std::string address_by_activity;
	const bool registration_by_VAT;
	const std::string grounds_for_not_charging_VAT;

	/*
	const std::string issuer_bulstat_no_vat //ИН по ДДС(BGЕИК - юридическо лице; BGЕГН - физическо лице).Задължителен за регистрирани по ДДС
	*/

	const std::string bulstat;
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

struct AggregatedAmounts
{
	const char* payment_type{ "B" };
	double total_amount {0};
	double payment_amount {0};
	//const std::string vat_rate;
	const char* original {"Y"};
	Date taxEventDate;
};

struct Invoice
{
	Invoice(const TiXmlDocument& monthNotifRoot, const User& user);

	const std::string fin_document_type_code; //INVOICE, DT_NOTIF, CT_NOTIF

	std::string fin_document_no; //leading zeroes! input by user !!!!!!!!!!!

	const std::string fin_document_month_no; //monthly_notification_num

	Date fin_document_date; //input by user !!!!!!!!!!!!!!!

	std::optional<MainDocument> mainDocument; //for credit and debit note

	const Recipient& recipient;
	const Issuer issuer;

	const int activityTypeCode;
	std::string health_insurance_fund_type_code;
	
	
	const Date date_from;
	const Date date_to;

	std::vector<BusinessOperation> businessOperations;

	AggregatedAmounts aggragated_amounts;



};