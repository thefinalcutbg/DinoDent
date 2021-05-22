#pragma once
#include "Manipulation.h"
#include "../jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"

class ManipulationParser
{
	Json::FastWriter writer;

public:
	std::string write(Manipulation m);
	void parse(const std::string& jsonString, Manipulation& m);
};

