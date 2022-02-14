#pragma once
#include <string>
#include <vector>
#include <optional>

#include "Model/Date.h"
#include "Model/Procedure/ProcedureTemplate.h"

constexpr const char* doctorPrefix = u8"д-р ";

struct Doctor
{
	int rowID;
	std::string LPK;
	std::string fname;
	std::string mname;
	std::string lname;
	std::string egn;
	int specialty{ -1 };
	std::string pass;
	int dentalServiceType() const
	{
		if (specialty == 60 || specialty == 64) return 0;
		if (specialty == 61 || specialty == 62 || specialty == 68) return 1;
		return -1;
	}

	std::string getFullName(bool prefix = true) const
	{
		return prefix ?
			doctorPrefix + fname + " " + lname
			:
			fname + " " + lname;
	}

};

struct NzokContract
{
	std::string contract_no;
	Date date;
	std::string bank;
	std::string bic;
	std::string iban;
};

constexpr const char* legalEntities[6]
{
	u8"Свободна професия",
	u8"ЕТ",
	u8"ЕООД",
	u8"ООД",
	u8"ЕАД",
	u8"АД"
};

struct Practice
{
	int rowID;
	std::string rziCode;
	std::string name;
	std::string bulstat;
	std::string firm_address;
	std::string practice_address;
	bool vat;
	int legal_entity;

	std::optional<NzokContract> nzok_contract;

	std::vector<ProcedureTemplate> priceList;};

struct User
{
	Doctor doctor;
	Practice practice;
};

