#include "DiagnosisContainer.h"
#include "JsonCpp/json.h"
#include <fstream>

inline void diagListParse(DiagnosisList& list, const Json::Value& jsonValue)
{
	list.reserve(jsonValue.size());

	for (auto& jV : jsonValue)
		list.emplace_back(jV.asString());
}

void DiagnosisContainer::initialize()
{
	if (s_init) return;

	std::ifstream ifs("diagnosis.json");
	Json::Reader reader;
	Json::Value jDiagList;

	reader.parse(ifs, jDiagList);

	diagListParse(s_caries, jDiagList["caries"]);
	diagListParse(s_pulpitis, jDiagList["pulpitis"]);
	diagListParse(s_lesion, jDiagList["periodontitis"]);
	diagListParse(s_fracture, jDiagList["fracture"]);
	diagListParse(s_root, jDiagList["radix"]);

	s_init = true;
}
