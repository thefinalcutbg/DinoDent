#include "DiagnosisContainer.h"
#include <JsonCpp/json.h>
#include <array>
#include <fstream>

bool s_init{ false };

std::array<std::vector<std::string>, 5> s_diagnosis;


inline void diagListParse(std::vector<std::string>& list, const Json::Value& jsonValue)
{
	list.reserve(jsonValue.size());

	for (auto& jV : jsonValue)
		list.emplace_back(jV.asString());
}

void DiagnosisContainer::initialize()
{
	if (s_init) return;

	std::ifstream ifs("data/diagnosis.json");
	Json::Reader reader;
	Json::Value jDiagList;

	reader.parse(ifs, jDiagList);

	diagListParse(s_diagnosis[DiagnosisType::Caries], jDiagList["caries"]);
	diagListParse(s_diagnosis[DiagnosisType::Pulpitis], jDiagList["pulpitis"]);
	diagListParse(s_diagnosis[DiagnosisType::Lesion], jDiagList["periodontitis"]);
	diagListParse(s_diagnosis[DiagnosisType::Fracture], jDiagList["fracture"]);
	diagListParse(s_diagnosis[DiagnosisType::Root], jDiagList["radix"]);

	s_init = true;
}

std::vector<std::string>& DiagnosisContainer::diagnosis(DiagnosisType type)
{
	return s_diagnosis[type];
}
