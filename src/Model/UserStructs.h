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
	int rowID{ 0 };
	std::string LPK;
	std::string fname;
	std::string mname;
	std::string lname;
	NhifSpecialty specialty{ NhifSpecialty::None };
	std::string pass;
	std::string phone;
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

		auto idx = static_cast<int>(specialty);

		if (idx < 0 || idx > 4) idx = 0;

		return specialtyToInt[idx];
	}

};

struct NhifContract
{
	std::string contract_no;
	Date date;
	std::string name_short;
	std::string nra_pass;
	std::string nssi_pass;
	std::string dentalTechnicianCode; //Dental Technician Code
	std::string getRhif() const {
		
		if (contract_no.size() < 2) return std::string{};

		return contract_no.substr(0, 2);

	}
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
	std::string bulstat; //issuer
	std::string firm_address; //issuer
	Ekatte practice_address;
	std::string street_address; //заради тъпата НЗОК
	bool hasVat{ false }; //issuer
	std::string pass;
	int legal_entity{ 0 }; //issuer
	std::string selfInsuredId; //issuer
	std::string bank;
	std::string iban;
	std::string bic;

	Settings settings;

	std::optional<NhifContract> nhif_contract;

	std::string addresssByActivity() const {
		return practice_address.getAddressWithStreet(street_address);
	}

	bool generateMonthlySheets() const {
		return nhif_contract.has_value() || settings.preferMonthlySheets;
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
               Ekatte::s_unfavList.size();
	}

};

