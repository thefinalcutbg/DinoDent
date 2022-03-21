#pragma once
#include <string>
#include <variant>
#include <optional>

struct User;

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

	const std::optional<std::string> registration_by_VAT;
	const std::string grounds_for_not_charging_VAT;

	const std::string issuer_bulstat_no_vat;
	const std::string bulstat;
};