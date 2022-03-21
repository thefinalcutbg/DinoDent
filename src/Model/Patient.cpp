#include "Patient.h"
#include "CityCode.h"

bool Patient::getSexFromEgn(const std::string& egn)
{
	return stoi(egn.substr(8, 1)) % 2 != 0;
}



int Patient::getAge(const Date& currentDate) const
{
	int age = currentDate.year - birth.year;

	if ((currentDate.month == birth.month && currentDate.day < birth.day)  || 
		birth.month > currentDate.month) 
		age--;

	return std::max(-1, age);
}


bool Patient::isAdult(const Date& currentDate) const
{
	return getAge(currentDate) > 17;
}

Date Patient::turns18At() const
{
	return Date{ birth.day, birth.month, birth.year + 18 };
}

std::string Patient::fullName() const
{
	std::string fullName;
	fullName.reserve(FirstName.length() + MiddleName.length() + LastName.length() + 2);

	fullName += FirstName;
	fullName += " ";
	if (MiddleName.length())
	{
		fullName += MiddleName;
		fullName += " ";
	}
	fullName += LastName;

	return fullName;
}

std::string Patient::firstLastName() const
{
	return FirstName + " " + LastName;
}

std::string Patient::getFullAddress() const
{
	std::string fullAddress;

	for (int i = 0; i < city.size(); i++)
	{
		if (city[i] == '(')
		{
			fullAddress = u8"с. " + city.substr(0, i);
			break;
		}
		else if (city[i] == ',')
		{
			fullAddress = u8"гр. " + city.substr(0, i);
			break;
		}
	}

	if (address != "")
		fullAddress.append(", " + address);

	return fullAddress;
}


Patient::~Patient()
{

}
