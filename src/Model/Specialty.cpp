#include "Specialty.h"

#include "Resources.h"
#include "json/json.h"

void Specialty::initialize()
{
	Json::Value jsonSpec;

	Json::Reader().parse(
		Resources::fromPath(":/json/json_specialty.json"),
		jsonSpec
	);

	s_specMap.reserve(jsonSpec.size());

	for (auto& val : jsonSpec)
	{
		std::string name = val["name"].asString();

		bool isDoctor = val["doc"] == 1;

		int index = val["code"].asInt();

		if (isDoctor) s_isDoctor.insert(index);

		s_specMap[index] = name;

		s_specNames[name] = val["code"].asInt();
	}

}

const std::string& Specialty::getName() const
{
	if (!s_specMap.count(m_idx)) return s_spec_dummy;

	return s_specMap[m_idx];
}



Specialty::Specialty(const std::string& name)
{
	if (!s_specNames.count(name)) return;

	m_idx = s_specNames[name];
}
