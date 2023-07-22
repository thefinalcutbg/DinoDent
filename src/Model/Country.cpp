#include "Country.h"
#include "Resources.h"
#include <JsonCpp/json.h>

struct CountryData
{
	std::string code;
	std::string name;
};

std::vector<CountryData> s_data;

Country::Country(const std::string& code)
{
	for (int i = 0; i < s_data.size(); i++)
	{
		if (code == s_data[i].code) {
			m_idx = i;
			return;
		}
	}
}

Country::Country(int index) : m_idx(index)
{
	if (index < 0 || index >= s_data.size()) m_idx = 0;
}

const std::string& Country::getCode() const
{
	return s_data[m_idx].code;
}

const std::string& Country::getName() const
{
	return s_data[m_idx].name;
}


std::vector<std::string> Country::getCodeCountryPair()
{
	std::vector<std::string> result;

	for (auto& data : s_data)
	{
		result.push_back(data.code + " - " + data.name);
	}

	return result;
}

void Country::initialize()
{
	Json::Value jsonCountries;

	Json::Reader().parse(Resources::fromPath(":/json/json_country.json"), jsonCountries);

	for (auto& value : jsonCountries)
	{
		s_data.push_back({ value["code"].asString(), value["name"].asString() });
	}
	
}
