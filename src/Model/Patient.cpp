#include "Patient.h"


Patient::Patient() : type(1), sex(0)
{
}

bool Patient::getSexFromEgn(const std::string& egn)
{
	int sexDigit = stoi(egn.substr(8, 1));

	return 
		(sexDigit % 2 == 0) ?
		false
		:
		true;
}

int Patient::getAge(Date currentDate) const
{
	const int& day = birth.day;
	const int& month = birth.month;
	const int& year = birth.year;

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

int Patient::getAge() const
{
	return getAge(Date::CurrentDate());
}


bool Patient::isAdult() const
{
	return getAge() > 17;
}

bool Patient::isAdult(Date currentDate) const
{
	return getAge(currentDate) > 17;
}

Patient::~Patient()
{

}
