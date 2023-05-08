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
#include "Model/MedicalStatuses.h"
typedef std::array<std::string, 32>TeethNotes;


struct Patient
{
	enum Sex { Male = 0, Female = 1 };

	long long rowid{ 0 };

	int type{ 1 };
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

	MedicalStatuses medStats;

	TeethNotes teethNotes;

	std::optional<std::vector<Procedure>> PISHistory;
	std::optional<InsuranceStatus> insuranceStatus;
	static Sex getSexFromEgn(const std::string& egn);
	int getAge(const Date& currentDate = Date::currentDate())  const;
	bool isAdult(const Date& currentDate = Date::currentDate())  const;
	Date turns18At() const;
	std::string fullName() const;
	std::string firstLastName() const;
	std::string getFullAddress() const;
	bool canBePregnant(const Date& = Date::currentDate()) const;
	~Patient();
};
