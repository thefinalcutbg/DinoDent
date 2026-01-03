#include "ProcedureCode.h"
#include "Resources.h"
#include <json/json.h>

std::vector<ProcedureCode> ProcedureCode::s_sorted_codes;

void ProcedureCode::initialize()
{
	Json::Reader reader;
	Json::Value pJson;

	reader.parse(Resources::fromPath(":/json/json_procedures.json"), pJson);

	s_mapping.reserve(pJson.size());
	s_sorted_codes.reserve(pJson.size());
	s_nameToCode.reserve(pJson.size());

	for (auto& j : pJson)
	{
		auto code = j["code"].asString();

		s_mapping[code] =
			ProcedureCode::Numenclature{
				.type = static_cast<ProcedureType>(j["type"].asInt()),
				.name = j["name"].asString(),
				.nhifCode = j["oldCode"].asInt(),
				.hisType = j["hisType"].asInt(),
				.isLegacy = j["legacy"].asBool(),
				.icd = j["icd"].asString()
		};

		s_nameToCode[j["name"].asString()] = code;

		if (j.isMember("ksmp")) {
			legacy_achi[j["oldCode"].asInt()] = j["ksmp"].asString();
		}

		if(code[0] != 'D'){
			s_sorted_codes.emplace_back(code);
		}
	}

}


std::vector<ProcedureCode> ProcedureCode::getByType(ProcedureType t)
{
	std::vector<ProcedureCode> result;

	for (auto& code : s_mapping)
	{
		if (code.second.type == t)
			result.push_back(code.first);
	}

	return result;
}

bool ProcedureCode::isValid() const
{
	if (m_code.empty()) return false;

	return s_mapping.count(m_code);
}

ProcedureCode::ProcedureCode(const std::string& code) : m_code(code)
{}

ProcedureCode::ProcedureCode(int nhifCode)
{
	static const std::unordered_map<int, std::string> nhifMapping =
	{
		{101, "D-01-001"},
		{102, "D-01-002"},
		{103, "D-01-003"},
		{520, "D-01-004"},
		{544, "D-01-006"},
		{901, "D-01-007"},
		{902, "D-01-008"},
		{903, "D-01-009"},
		{301, "D-02-001"},
		{332, "D-03-001"},
		{333, "D-03-002"},
		{508, "D-04-001"},
		{509, "D-04-002"},
		{510, "D-04-003"},
		{832, "D-09-002"},
		{833, "D-09-003"},
		{834, "D-09-004"},
		{835, "D-09-005"}
	};

	if (!nhifMapping.count(nhifCode)) {
		return;
	}

	m_code = nhifMapping.at(nhifCode);
}

ProcedureCode ProcedureCode::fromName(const std::string & name)
{
	if (!s_nameToCode.count(name)) return ProcedureCode();

	return ProcedureCode(s_nameToCode.at(name));
}

const std::string& ProcedureCode::ACHICode() const
{
	if (!isValid()) return dummy;

	if ((isLegacy() || m_code[0] == 'D') && nhifCode()) {
		return legacy_achi.at(nhifCode());
	}

	return m_code;
}

const std::string& ProcedureCode::defaultICD10() const
{
	if (!isValid()) return dummy;

	return s_mapping.at(m_code).icd;
}

int ProcedureCode::achiBlock() const
{
	return isValid() ? s_mapping[m_code].hisType : 0;
}

const std::string& ProcedureCode::achiBlockName() const
{
	static const std::vector<std::pair<int, std::string>> s_achi_block = {

	{450,	"Дентални прегледи"},
	{451,	"Дентални рентгенови изследвания и интерпретация"},
	{452,	"Други дентални диагностични процедури"},
	{453,	"Дентална профилактика и избелване"},
	{454,	"Нехирургично/Неусложнено отстраняване на зъби"},
	{455,	"Хирургично отстраняване на зъби" },
	{456,	"Пародонтални интервенции"},
	{457,	"Дентоалвеоларни и пародонтални хирургични процедури"},
	{458,	"Перирадикуларна хирургия"},
	{459,	"Хирургични процедури на устна кухина. Орално-хирургични процедури"},
	{460,	"Дентоалвеоларна травматология"},
	{461,	"Хирургични имплантологични процедури"},
	{462,	"Аугментационни процедури"},
	{463,	"Анестезия за дентално или хирургично лечение"},
	{464,	"Консервативно зъболечение. Ендодонтия. Ендодонтски процедури"},
	{465,	"Консервативно зъболечение. Възстановяване чрез метал"},
	{466,	"Консервативно зъболечение. Възстановяване с цвета на зъба"},
	{467,	"Други възстановителни дентални процедури"},
	{468,	"Протетични възстановителни процедури"},
	{469,	"Допълнителни дентални процедури в неснемаемото протезиране"},
	{470,	"Процедури при протезиране върху импланти"},
	{471,	"Процедури при снемаемото протезиране"},
	{472,	"Лицево-челюстно протезиране"},
	{473,	"Други процедури в снемаемото протезиране"},
	{474,	"Поправки на протези"},
	{475,	"Други протетични процедури"},
	{476,	"Ортодонтия. Снемаеми ортодонтски апарати"},
	{477,	"Ортодонтия. Фиксирани ортодонтски апарати"},
	{478,	"Ортодонтия. Екстраорални и други ортодонтски апарати"},
	{479,	"Ортодонтия. Други ортодонтски дейности"},
	{480,	"Палиативни дентални грижи"},
	{481,	"Дентална лекарствена терапия"},
	{482,	"Оклузална терапия"},
	{490,	"Различни дентални дейности"},
	{1910,	"Мозъчна анестезия"},
	{1915,	"Други интервенции в помощ на пациента"}

	};

	auto blockIdx = achiBlock();

	for (auto& [idx, name] : s_achi_block) {

		if (blockIdx == idx) return name;

	}

	return dummy;
}

ProcedureType ProcedureCode::type() const
{
	return isValid() ? s_mapping[m_code].type : ProcedureType::General;
}

ProcedureScope ProcedureCode::getScope() const
{
	return getScope(type());
}

bool ProcedureCode::isLegacy() const
{
	if (s_mapping.count(m_code)) {

		return s_mapping.at(m_code).isLegacy;
	}

	return false;
	
}

ProcedureScope ProcedureCode::getScope(ProcedureType t)
{
	static const std::vector<std::pair<ProcedureType, ProcedureScope>> pairs = {

	{ ProcedureType::General, ProcedureScope::AllOrNone},
	{ ProcedureType::FullExam, ProcedureScope::AllOrNone},
	{ ProcedureType::Depuratio, ProcedureScope::AllOrNone},
	{ ProcedureType::DenturePair, ProcedureScope::AllOrNone},
	{ ProcedureType::Anesthesia, ProcedureScope::AllOrNone},

	{ ProcedureType::Restoration, ProcedureScope::SingleTooth},
	{ ProcedureType::RemoveRestoration, ProcedureScope::SingleTooth},
	{ ProcedureType::ToothNonSpecific, ProcedureScope::SingleTooth},
	{ ProcedureType::DepuratioTooth, ProcedureScope::SingleTooth},
	{ ProcedureType::Extraction, ProcedureScope::SingleTooth},
	{ ProcedureType::Implant, ProcedureScope::SingleTooth},
	{ ProcedureType::Endodontic, ProcedureScope::SingleTooth},
	{ ProcedureType::Post, ProcedureScope::SingleTooth},
	{ ProcedureType::RemovePost, ProcedureScope::SingleTooth},
	{ ProcedureType::PostCore, ProcedureScope::SingleTooth},
	{ ProcedureType::PostCrown, ProcedureScope::SingleTooth},

	{ ProcedureType::Bridge, ProcedureScope::Range},
	{ ProcedureType::RemoveCrownOrBridge, ProcedureScope::Range},
	{ ProcedureType::Denture, ProcedureScope::Range},
	{ ProcedureType::Splint, ProcedureScope::Range},
	{ ProcedureType::DepuratioQuadrant, ProcedureScope::Range},
	{ ProcedureType::MultipleExtraction, ProcedureScope::Range},

	{ ProcedureType::Crown, ProcedureScope::Ambi},
	{ ProcedureType::CrownOrBridgeOrVeneer, ProcedureScope::Ambi}
	};

	for (auto& [type, effect] : pairs) {

		if (t == type) return effect;

	}

	return ProcedureScope::AllOrNone;
}

int ProcedureCode::nhifCode() const
{
    return isValid() ? s_mapping[m_code].nhifCode : 0;
}

bool ProcedureCode::isNhifOnly() const
{
    auto nhif_code = nhifCode();

    return nhif_code && (nhif_code < 832 && nhif_code > 835);
}

bool ProcedureCode::isFixedProsthoBased() const
{
	auto t = type();

	if (t == ProcedureType::Bridge) return true;
	if (t == ProcedureType::Splint) return true;
	if (t == ProcedureType::RemoveCrownOrBridge) return true;

	return false;
}

const std::string& ProcedureCode::name() const
{
	return isValid() ? s_mapping[m_code].name : dummy;
}
