#pragma once
#include <string>
#include <vector>
#include <optional>

#include "Model/Date.h"
#include "Model/Procedure/ProcedureTemplate.h"

struct Doctor
{
	std::string LPK;
	std::string pass;
	std::string doctor_name;
	int specialty{ -1 };
	int dentalServiceType() const
	{
		if (specialty == 60 || specialty == 64) return 0;
		if (specialty == 61 || specialty == 62 || specialty == 68) return 1;
		return -1;
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
	std::string rziCode;
	std::string practice_name;
	std::string bulstat;
	std::string firm_address;
	std::string practice_address;
	bool vat;
	int legal_entity;

	std::optional<NzokContract> nzok_contract;

	std::vector<ProcedureTemplate> priceList;
};

struct User : public Doctor, public Practice {};

