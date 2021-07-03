#include "CustomProcedures.h"

#include <array>
#include <fstream>

#include "c:/Dev/TorqueMVP/jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"

CustomProcedures CustomProcedures::_instance;

CustomProcedures::CustomProcedures()
{
}

CustomProcedures& CustomProcedures::instance()
{
	return _instance;
}

void CustomProcedures::loadCustomProcedures()
{
	std::ifstream ifs("manipulation.json");
	Json::Reader reader;
	Json::Value m;

	reader.parse(ifs, m);

	const Json::Value& manipulation = m["manipulation"];

	_customProcedures.clear();
	_customProcedures.reserve(manipulation.size());

	for (int i = 0; i < manipulation.size(); i++)
	{
		ProcedureTemplate m;
		m.type = static_cast<ProcedureType>(manipulation[i]["type"].asInt());
		m.code = manipulation[i]["code"].asInt();
		m.name = manipulation[i]["name"].asString();
		m.price = manipulation[i]["price"].asDouble();

		if (!manipulation[i]["default_diag"].isNull())
		{
			m.diagnosis = manipulation[i]["default_diag"].asString();
		}
		if (!manipulation[i]["material"].isNull())
		{
			m.material = manipulation[i]["material"].asString();
		}

		_customProcedures.emplace_back(m);
	}

}

const std::vector<ProcedureTemplate>& CustomProcedures::getCustomProcedures()
{
	return _customProcedures;
}
