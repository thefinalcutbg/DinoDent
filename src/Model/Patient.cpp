#include "Patient.h"

bool Patient::getSexFromEgn(const std::string& egn)
{
	int sexDigit = stoi(egn.substr(8, 1));

	return 
		(sexDigit % 2 == 0) ?
		false
		:
		true;
}

int Patient::getAge(Date currentDate)
{
	int& day = birth.day;
	int& month = birth.month;
	int& year = birth.year;

	int& daynow = currentDate.day;
	int& monthnow = currentDate.month;
	int& yearnow = currentDate.year;

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

int Patient::getAge()
{
	return getAge(Date::getCurrentDate());
}


bool Patient::isAdult()
{
	return getAge() > 17;
}

bool Patient::isAdult(Date currentDate)
{
	return getAge(currentDate) > 17;
}
