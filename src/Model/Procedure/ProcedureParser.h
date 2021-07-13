#pragma once
#include "Procedure.h"
#include "../jsoncpp/jsoncpp-src-0.5.0/include/json/json.h"

class ProcedureParser
{
	Json::FastWriter writer;

public:
	std::string write(const Procedure& m);
	void parse(const std::string& jsonString, Procedure& m);
};

