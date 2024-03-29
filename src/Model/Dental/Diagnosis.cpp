#include "Diagnosis.h"
#include <json/json.h>

#include "Resources.h"

inline void diagListParse(std::vector<std::string>& list, const Json::Value& jsonValue)
{
	list.reserve(jsonValue.size());

	for (auto& jV : jsonValue)
		list.emplace_back(jV.asString());
}

void Diagnosis::initialize()
{
	s_names.clear();

	Json::Reader reader;
	Json::Value jDiagList;

	reader.parse(Resources::defaultDiagnosisListJson(), jDiagList);
	
	s_names = std::vector<std::string>(jDiagList.size());

	for (auto& json : jDiagList) {
		
		s_names[json["key"].asInt()] = json["name"].asString();

	}
}

Diagnosis::Diagnosis(const std::string& name)
{
    for (std::size_t i = 0; i < s_names.size(); i++)
	{
		if (name == s_names[i]) {
			m_idx = i;
			return;
		}
	}

	description = name;
	
}

Diagnosis::Diagnosis(int key, const std::string& description) : m_idx{ key }, description(description)
{
	if (!isValid()) m_idx = 0;
}

bool Diagnosis::isValid() const
{
	if (!m_idx) {
		return description.size() ? true : false;
	}

	return m_idx < s_names.size();
}

const std::string& Diagnosis::getFullDiagnosis() const
{
	if (description.size()) return description;

	return s_names[m_idx];

}
