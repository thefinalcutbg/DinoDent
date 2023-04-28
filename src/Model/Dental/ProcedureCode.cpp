#include "ProcedureCode.h"
#include "Resources.h"
#include <JsonCpp/json.h>
#include <algorithm>

void ProcedureCode::initialize()
{
	Json::Reader reader;
	Json::Value pJson;

	reader.parse(Resources::fromPath(":/json/json_procedures.json"), pJson);

	for (auto& j : pJson)
	{

		s_mapping[j["code"].asString()] =
			ProcedureCode::Numenclature{
				.type = static_cast<ProcedureType>(j["type"].asInt()),
				.name = j["name"].asString(),
				.ksmp = j.isMember("ksmp") ? j["ksmp"].asString() : "",
				.oldCode = j["oldCode"].asInt()
		};

		if (j.isMember("ksmp")) {
			nhif_procedures[j["oldCode"].asInt()] = j["code"].asString();
		}
	}

}

std::vector<ProcedureCode> ProcedureCode::getNonNhifProcedures()
{
	std::vector<ProcedureCode> result;

	for (auto& kv : s_mapping)
	{
			result.push_back(kv.first);
	}

	std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {return lhs.code() < rhs.code();});

	return result;
}

ProcedureCode::ProcedureCode(const std::string& code) : m_code(code)
{}

ProcedureCode::ProcedureCode(int oldCode) : m_code(nhif_procedures[oldCode])
{}

ProcedureType ProcedureCode::type() const
{
	return isValid() ? s_mapping[m_code].type : ProcedureType::none;
}

int ProcedureCode::oldCode() const
{
	return isValid() ? s_mapping[m_code].oldCode : 0;
}

const std::string& ProcedureCode::ksmp() const
{
	return isValid() ? s_mapping[m_code].ksmp : dummy;
}

const std::string& ProcedureCode::name() const
{
	return isValid() ? s_mapping[m_code].name : dummy;
}