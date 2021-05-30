#pragma once
#include <string>
#include "Date.h"

struct Patient
{
	Patient() : type(1), sex(0){};


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

	int getAge();
	int getAge(Date currentDate);

	bool isAdult();
	bool isAdult(Date currentDate);
};
