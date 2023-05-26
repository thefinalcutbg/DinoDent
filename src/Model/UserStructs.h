#pragma once
#include <string>
#include <vector>
#include <optional>

#include "Model/Date.h"
#include "Model/Settings.h"
#include "Model/Dental/ProcedureCode.h"
#include "Model/Specialty.h"
#include "Model/Ekatte.h"

constexpr const char* doctorPrefix = "д-р ";

enum class NhifSpecialty {None, General, Pediatric, OralSurgeon, Maxillofacial};

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
	NhifSpecialty specialty{ NhifSpecialty::None };
	std::string pass;
	std::string phone;
	bool severalRHIF{ false };
	Specialty hisSpecialty;

	std::string getFullName(bool prefix = true) const
	{
		return prefix ?
			doctorPrefix + fname + " " + lname
			:
			fname + " " + lname;
	}

	int specialtyAsInt() const {

		static constexpr int specialtyToInt[5]{ 0, 64, 61, 62, 68 };

		return specialtyToInt[static_cast<int>(specialty)];
	}

};

struct NhifContract
{
	std::string contract_no;
	Date date;
	std::string name_short;
	bool unfavourable{ false };
	std::string nra_pass;
	std::string nssi_pass;
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
	Ekatte practice_address;
	std::string vat;
	std::string pass;
	int legal_entity{ 0 };
	std::string selfInsuredId;

	Settings settings;

	std::optional<NhifContract> nhif_contract;

	std::string RHIF() const {
		return rziCode.substr(0, 2);
	}

	bool hasNraAccess() const {
		return 
			nhif_contract.has_value() &&
			nhif_contract->nra_pass.size();
	}

	bool hasNssiAccess() const {
		return 
			nhif_contract.has_value() &&
			nhif_contract->nssi_pass.size();
	}

	bool isUnfavourable() const {
		return 
			nhif_contract && 
			nhif_contract->unfavourable;
	}

};

