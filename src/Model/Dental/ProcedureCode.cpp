#include "ProcedureCode.h"
#include "Resources.h"
#include <json/json.h>
#include <algorithm>
#include <type_traits>
#include <QDebug>
void ProcedureCode::initialize()
{
	Json::Reader reader;
	Json::Value pJson;

	reader.parse(Resources::fromPath(":/json/json_procedures.json"), pJson);

	s_mapping.reserve(pJson.size());
	s_sorted_codes.reserve(pJson.size());

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

ProcedureCode::ProcedureCode(int nhifCode) : m_code(legacy_achi[nhifCode])
{}

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
	{464,	"Консервативно зъболечение. Ендодонтия.Ендодонтски процедури"},
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
	{476,	"Ортодонтия.снемаеми ортодонтски апарати"},
	{477,	"Ортодонтия.фиксирани ортодонтски апарати"},
	{478,	"Ортодонтия.екстраорални и други ортодонтски апарати"},
	{479,	"Ортодонтия.други ортодонтски дейности"},
	{480,	"Палиативни дентални грижи"},
	{481,	"Дентална лекарствена терапия"},
	{482,	"Оклузална терапия"},
	{483,	"Различни дентални дейности"},
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
	static const std::vector<std::pair<ProcedureType, ProcedureScope>> pairs = {

		{ ProcedureType::General, ProcedureScope::AllOrNone},
		{ ProcedureType::FullExam, ProcedureScope::AllOrNone},
		{ ProcedureType::Depuratio, ProcedureScope::AllOrNone},
		{ ProcedureType::DenturePair, ProcedureScope::AllOrNone},
		{ ProcedureType::Anesthesia, ProcedureScope::AllOrNone},

		{ ProcedureType::Restoration, ProcedureScope::SingleTooth},
		{ ProcedureType::ToothSpecific, ProcedureScope::SingleTooth},
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

	auto pType = type();

	for (auto& [type, effect] : pairs) {

		if (pType == type) return effect;

	}

	return ProcedureScope::AllOrNone;
}

bool ProcedureCode::isLegacy() const
{
	if (s_mapping.count(m_code)) {

		return s_mapping.at(m_code).isLegacy;
	}

	return false;
	
}

int ProcedureCode::nhifCode() const
{
	return isValid() ? s_mapping[m_code].nhifCode : 0;
}

const std::string& ProcedureCode::name() const
{
	return isValid() ? s_mapping[m_code].name : dummy;
}
