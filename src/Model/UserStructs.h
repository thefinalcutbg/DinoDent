#pragma once
#include <string>
#include <vector>
#include <optional>

#include "Model/Date.h"
#include "Model/Settings.h"
#include "Model/Dental/ProcedureTemplate.h"

constexpr const char* doctorPrefix = "д-р ";

enum class NhifSpecialty {None, General, Pediatric, OralSurgeon, Maxillofacial};

constexpr int specialtyToInt[5]{ 0, 64, 61, 62, 68 };

struct PracticePair
{
	std::string rzi;
	std::string name;
	std::string pass;
};

struct PracticeDoctor
{
	std::string lpk;
	std::string name;
	bool admin;
	NhifSpecialty specialty;
};

struct Doctor
{
	int rowID;
	std::string LPK;
	std::string fname;
	std::string mname;
	std::string lname;
	std::string egn;
	NhifSpecialty specialty{ NhifSpecialty::None };
	std::string pass;
	std::string phone;
	bool severalRHIF{ false };

	int dentalServiceType() const
	{
		switch (specialty) {
			case NhifSpecialty::None: return -1;
			case NhifSpecialty::General: return 0;
			default: return 1;
		}
	}

	std::string getFullName(bool prefix = true) const
	{
		return prefix ?
			doctorPrefix + fname + " " + lname
			:
			fname + " " + lname;
	}

	int specialtyAsInt() const {
		return specialtyToInt[static_cast<int>(specialty)];
	}

};

struct NzokContract
{
	std::string contract_no;
	Date date;
	std::string name_short;
	std::string nra_pass;
	std::string bank;
	std::string bic;
	std::string iban;
};

constexpr const char* legalEntities[6]
{
	"Свободна професия",
	"ЕТ",
	"ЕООД",
	"ООД",
	"ЕАД",
	"АД"
};

struct Practice
{
	int rowID;
	std::string rziCode;
	std::string name;
	std::string bulstat;
	std::string firm_address;
	std::string practice_address;
	std::string vat;
	std::string pass;
	int legal_entity{ 0 };

	Settings settings;

	std::optional<NzokContract> nzok_contract;

	std::vector<ProcedureTemplate> priceList;

	std::string RHIF() const {
		return rziCode.substr(0, 2);
	}

	bool hasNraAccess() const {
		return nzok_contract.has_value() && !
			nzok_contract->nra_pass.empty();
	}

};

