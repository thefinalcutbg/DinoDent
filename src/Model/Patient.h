#pragma once
#include <string>
#include "Date.h"
#include <memory>
#include <array>
#include "Model/Dental/Procedure.h"
#include "InsuranceStatus.h"
#include <optional>
#include <vector>
#include "Model/Ekatte.h"
#include "Model/Foreigner.h"
#include "Model/MedicalStatuses.h"
#include "Model/Allergy.h"

typedef std::array<std::string, 32>TeethNotes;

struct Patient
{
	enum Type { EGN = 1, LNCH = 2, SSN = 3, EU = 4 };

	enum Sex { Male = 0, Female = 1 };

	long long rowid{ 0 };

	Type type{ Type::EGN };
	std::string id;
	Date birth;

	Sex sex{ Male };

	std::string FirstName;
	std::string MiddleName;
	std::string LastName;
	Ekatte city;
	std::string address;
	std::string HIRBNo;
	std::string phone;

	std::optional<Foreigner> foreigner;

	MedicalStatuses medStats;

	std::vector<Allergy> allergies;

	TeethNotes teethNotes;
	std::string patientNotes;

	std::string colorNameRgb;

	std::optional<std::vector<Procedure>> PISHistory;
	std::optional<std::vector<Procedure>> HISHistory;
	std::optional<InsuranceStatus> insuranceStatus;
	static Sex getSexFromEgn(const std::string& egn);
	int getAge(const Date& currentDate = Date::currentDate())  const;
	bool isAdult(const Date& currentDate = Date::currentDate())  const;
	Date turns18At() const;
	std::string fullName() const;
	std::string firstLastName() const;
	std::string getFullAddress() const;
	std::string getAllergiesStr() const;
	std::string getMedStatusStr() const;
	std::string getDirName() const;
	bool canBePregnant(const Date& = Date::currentDate()) const;
	~Patient();
};
