#include "MKB.h"

#include <unordered_map>
#include <JsonCpp/json.h>

#include "Resources.h"

std::unordered_map<std::string, std::string> mkbToName;
const std::string empty{};

void MKB::initialize() {

	mkbToName.clear();
	Json::Value mkbjson;
	mkbjson.resize(40000);
	mkbToName.clear();

	Json::Reader reader;
	reader.parse(Resources::mkbJson(), mkbjson);
	mkbToName.reserve(mkbjson.size());

	for (auto& pair : mkbjson) mkbToName[pair["key"].asString()] = pair["value"].asString();


}


const std::string& MKB::getNameFromMKBCode(const std::string& mkbCode)
{
	if (mkbToName.count(mkbCode)) return mkbToName.at(mkbCode);

	return empty;
}

bool MKB::isInitialized()
{
	return !mkbToName.empty();
}
