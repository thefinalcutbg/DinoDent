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
		if (daynow >= day)
			age = yearnow - year;
		else
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
	return getAge(Date::currentDate());
}


bool Patient::isAdult() const
{
	return getAge() > 17;
}

bool Patient::isAdult(Date currentDate) const
{
	return getAge(currentDate) > 17;
}

Date Patient::turns18At() const
{
	return Date{ birth.day, birth.month, birth.year + 18 };
}

Patient::~Patient()
{

}
