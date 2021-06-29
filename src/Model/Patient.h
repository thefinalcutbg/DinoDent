#pragma once
#include <string>
#include "Date.h"

struct Patient
{
	int type{ 1 };
	std::string id;
	Date birth;

	bool sex{ 0 };

	std::string FirstName;
	std::string MiddleName;
	std::string LastName;
	std::string city;
	std::string address;
	std::string HIRBNo;
	std::string phone;
	std::string allergies;
	std::string currentDiseases;
	std::string pastDiseases;

	static bool getSexFromEgn(const std::string& egn);

	int getAge() const;
	int getAge(Date currentDate)  const;

	bool isAdult() const;
	bool isAdult(Date currentDate)  const;
	Date eighteenBirthday() const;
	~Patient();
};
