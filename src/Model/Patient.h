#pragma once
#include <string>
#include "Date.h"

struct Patient
{
	Patient() : type(1), sex(0){};


	int type;
	std::string id;
	std::string birth;
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

	static bool getSexFromEgn(const std::string& egn)
	{
		int sexDigit = stoi(egn.substr(8, 1));
		if (sexDigit % 2 == 0) return false;
		else return true;
	}

	static int getAge(const std::string& birthDate)
	{
		Date birth(birthDate);
		int &day = birth.day;
		int &month = birth.month;
		int& year = birth.year;
		
		int daynow = Date::currentDay();
		int monthnow = Date::currentMonth();
		int yearnow = Date::currentYear();

		int age = 0;

		if (month == monthnow)
		{
			if (day >= daynow)
				age = yearnow - year;
			else if (day < daynow)
				age = yearnow - year - 1;
		}
		else if (month > monthnow)
			age = yearnow - year - 1;

		else if (month < monthnow)
			age = yearnow - year;

		return age;
	}

};
