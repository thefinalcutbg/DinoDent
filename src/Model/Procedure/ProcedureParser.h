#pragma once
#include "Procedure.h"
#include "JsonCpp/json.h"

class ProcedureParser
{
	Json::FastWriter writer;

public:
	std::string write(const Procedure& m);
	void parse(const std::string& jsonString, Procedure& m);
};
