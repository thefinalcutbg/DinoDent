#include "ProcedureCode.h"
#include "Resources.h"
#include <json/json.h>
#include <algorithm>
#include <type_traits>

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
				.oldCode = j["oldCode"].asInt(),
				.hisType = j["hisType"].asInt(),
				.isLegacy = j["isLegacy"].asBool(),
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

ProcedureCode::ProcedureCode(const std::string& code) : m_code(code)
{}

ProcedureCode::ProcedureCode(int oldCode) : m_code(legacy_achi[oldCode])
{}

const std::string& ProcedureCode::ACHICode() const
{
	if (!isValid()) return dummy;

	if (isLegacy() && oldCode()) {
		return legacy_achi.at(oldCode());
	}

	return m_code;
}

int ProcedureCode::achiBlock() const
{
	return isValid() ? s_mapping[m_code].hisType : 0;
}

const std::string& ProcedureCode::achiBlockName() const
{
	static const std::unordered_map<int, std::string> s_achi_block{

	{450,	"Дентални прегледи"},
	{451,	"Дентални рентгенови изследвания и интерпретация"},
	{452,	"Други дентални диагностични процедури"},
	{453,	"Дентална профилактика и избелване"},
	{454,	"Нехирургично/Неусложнено отстраняване на зъби"},
	{455,	"Хирургично отстраняване на зъби" },
	{456,	"Пародонтални интервенции"},
	{457,	"Дентоалвеоларни и пародонтални хирургични процедури"},
	{458,	"Перирадикуларна хирургия"},
	{459,	"Хирургични процедури на устна кухина.орално-хирургични процедури"},
	{460,	"Дентоалвеоларна травматология"},
	{461,	"Хирургични имплантологични процедури"},
	{462,	"Аугментационни процедури"},
	{463,	"Анестезия за дентално или хирургично лечение"},
	{464,	"Консервативно зъболечение.ендодонтия.ендодонтски процедури"},
	{465,	"Консервативно зъболечение.възстановяване чрез метал"},
	{466,	"Консервативно зъболечение.възстановяване с цвета на зъба"},
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

	return s_achi_block.count(blockIdx) ? s_achi_block.at(blockIdx) : dummy;
}

ProcedureType ProcedureCode::type() const
{
	return isValid() ? s_mapping[m_code].type : ProcedureType::General;
}

int ProcedureCode::oldCode() const
{
	return isValid() ? s_mapping[m_code].oldCode : 0;
}

const std::string& ProcedureCode::name() const
{
	return isValid() ? s_mapping[m_code].name : dummy;
}
