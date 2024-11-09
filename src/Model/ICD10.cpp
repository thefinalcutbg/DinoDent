#include "ICD10.h"

#include <unordered_map>
#include <json/json.h>

#include "Resources.h"

namespace ICDPrv {
	std::unordered_map<std::string, std::string> icdToName;
	std::unordered_map<std::string, std::string> nameToIcd;
	const std::string dummy{};
	std::vector<ICD10> dentalMKB;
	std::vector<ICD10> fullMKB;
}

using namespace ICDPrv;

ICD10::ICD10(const std::string& code) :
	m_code(icdToName.count(code) ? code : std::string())
{}

const std::string& ICD10::name() const
{
	if (m_code.empty()) return dummy;

	if (icdToName.count(m_code)) return icdToName.at(m_code);

	return dummy;
}

bool ICD10::setCode(const std::string& code)
{
	if (icdToName.count(code)) {
		m_code = code;
		return true;
	}

	return false;
}

void ICD10::initialize() {

	icdToName.clear();
	Json::Value mkbjson;
	mkbjson.resize(40000);

	fullMKB.clear();
	dentalMKB.clear();
	icdToName.clear();

	Json::Reader reader;
	reader.parse(Resources::icdJson(), mkbjson);
	icdToName.reserve(mkbjson.size());
	nameToIcd.reserve(mkbjson.size());
	fullMKB.reserve(mkbjson.size());


	for (auto& pair : mkbjson) {

		auto code = pair["K"].asString();
		auto name = pair["V"].asString();

		icdToName[code] = name;
		nameToIcd[name] = code;

		if (pair.isMember("D")) {
			dentalMKB.push_back(code);
		}

		fullMKB.push_back(code);
	}

}

const std::string& ICD10::getDescriptionFromICDCode(const std::string& mkbCode)
{
	if (icdToName.count(mkbCode)) return icdToName.at(mkbCode);

	return dummy;
}

const std::string& ICD10::getCodeFromName(const std::string& icdDescription)
{
	if (nameToIcd.count(icdDescription)) {
		return nameToIcd.at(icdDescription);
	}

	return dummy;
}

bool ICD10::isInitialized()
{
	return !icdToName.empty();
}

const std::vector<ICD10>& ICD10::getDentalICDCodes()
{
	return dentalMKB;
}

const std::vector<ICD10>& ICD10::getFullMKBList()
{
	return fullMKB;
}
