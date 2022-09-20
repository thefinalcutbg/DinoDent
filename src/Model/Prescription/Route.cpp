#include "Route.h"
#include <JsonCpp/json.h>
#include "Resources.h"

std::vector<std::string> s_routeNum;

void Route::initialize()
{
	Json::Value jsonRoute;

	Json::Reader().parse(
		Resources::fromPath(":/json/json_route.json"),
		jsonRoute
	);

	s_routeNum = { jsonRoute.size()+1, std::string{} };

	for (auto& pair : jsonRoute)
	{
		s_routeNum[pair["key"].asInt()] = pair["value"].asString();
	}
}

bool Route::isValidStr(const std::string& valueStr)
{
	for (int i = 0; i < s_routeNum.size(); i++)
	{
		if (valueStr == s_routeNum[i]) return true;
	}

	return false;
}

const std::vector<std::string>& Route::getStringList()
{
	return s_routeNum;
}


bool Route::set(int key)
{
	if (key >= 0 && key < s_routeNum.size()) {
		m_key = key;
		return true;
	}

	return false;
}

bool Route::set(const std::string& valueStr)
{
	for (int i = 0; i < s_routeNum.size(); i++)
	{
		if (valueStr == s_routeNum[i])
		{
			m_key = i;
			return true;
		}
	}

	return false;
}

const std::string& Route::getValueStr() const
{
	return s_routeNum[m_key];
}
