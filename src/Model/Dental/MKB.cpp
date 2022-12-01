#include "MKB.h"

#include <unordered_map>
#include <JsonCpp/json.h>

#include "Resources.h"

namespace MKBPrv {
	std::unordered_map<std::string, std::string> mkbToName;
	const std::string empty{};
	std::vector<MKB> dentalMKB;
}

using namespace MKBPrv;


const std::string& MKB::name() const
{
	if (mkbToName.count(m_code)) return mkbToName.at(m_code);

	return empty;
}

bool MKB::setCode(const std::string& code)
{
	if (mkbToName.count(code)) {
		m_code = code;
		return true;
	}

	return false;
}

void MKB::initialize() {

	mkbToName.clear();
	Json::Value mkbjson;
	mkbjson.resize(40000);
	mkbToName.clear();

	Json::Reader reader;
	reader.parse(Resources::mkbJson(), mkbjson);
	mkbToName.reserve(mkbjson.size());

	for (auto& pair : mkbjson) {

		auto code = pair["key"].asString();

		mkbToName[code] = pair["value"].asString();
		
		if (code[0] == 'K' && std::stoi(code.substr(1,2)) < 20) {
			dentalMKB.emplace_back(code);
		}
	}


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

const std::vector<MKB>& MKB::getDentalMKBList()
{
	return dentalMKB;
}
