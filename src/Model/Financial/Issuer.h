#pragma once
#include <string>
#include <variant>
#include <optional>

struct Practice;
struct Doctor;

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
	//default construction with current user
	Issuer(); 

	IssuerType type;
	std::string company_name;
	std::string address_by_contract;
	std::string address_by_activity;

	std::optional<std::string> registration_by_VAT;
	std::string grounds_for_not_charging_VAT;

	std::string bulstat;

	std::string bank;
	std::string iban;
	std::string bic;

private:
	Issuer(const Practice& user, const Doctor& doctor);

};