#pragma once
#include <string>
#include "Date.h"

struct Patient
{
	Patient();


	int type;
	std::string id;
	Date birth;

	bool sex;

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

	~Patient();
};
