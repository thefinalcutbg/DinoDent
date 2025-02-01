#include "Patient.h"
#include "FreeFunctions.h"

Patient::Sex Patient::getSexFromEgn(const std::string& egn)
{
	return Sex(stoi(egn.substr(8, 1)) % 2 != 0);
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
	std::string cityStr = city.getString();

	if (address.empty()) return cityStr;

	for (int i = 0; i < cityStr.size(); i++)
	{
		if (cityStr[i] == ',')
		{
			cityStr = cityStr.substr(0, i);
			break;
		}
	}

	cityStr.append(", " + address);

	return cityStr;
}

std::string Patient::getAllergiesStr() const
{
	std::string result;

	for (auto& a : allergies) {
		result += a.description;
		result += ", ";
	}

	if (result.size()) {
		result.pop_back();
		result.pop_back();
	}
	else {
		result = "Не съобщава";
	};

	return result;
}

std::string Patient::getMedStatusStr() const
{
	//refactor this for non-nhis entities only

	std::string result;

	if (medStats.condition.size()) {

		result += "Настоящи заболявания: ";

		for (auto& s : medStats.condition) {
			result += s.diagnosis.code();
			result += ", ";
		}

		result.pop_back();
		result.pop_back();
	}

	if (medStats.history.size()) {

		if (result.size()) {
			result += ' ';
		}

		result += "Минали заболявания: ";

		for (auto& s : medStats.history) {
			result += s.diagnosis.code();
			result += ", ";
		}

		result.pop_back();
		result.pop_back();
	}

	return result;
}

std::string Patient::getDirName() const
{
	std::string result;

	result += FreeFn::toUpper(LastName) + " " + FirstName + " ";

	if (MiddleName.size()) {
		result += MiddleName;
		result += " ";
	}

	result += id.substr(0, 6);

	return result;
}

bool Patient::canBePregnant(const Date& date) const
{
	if (sex == Male) return false;

	auto age = getAge(date);

	return (age > 10 && age < 55);
}


Patient::~Patient()
{

}
