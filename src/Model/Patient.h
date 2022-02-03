#pragma once
#include <string>
#include "Date.h"
#include <memory>
#include <array>

typedef std::array<std::string, 32>TeethNotes;


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

	TeethNotes teethNotes;

	static bool getSexFromEgn(const std::string& egn);
	int getAge(const Date& currentDate = Date::currentDate())  const;
	bool isAdult(const Date& currentDate = Date::currentDate())  const;
	Date turns18At() const;
	std::string fullName() const;
	std::string firstLastName() const;
	const std::string& getCityAddress() const;
	

	~Patient();
};
