#pragma once
#include <string>

enum class ProcedureType { general, any, obturation, extraction, endo, crown, implant, fibersplint, removecrown, removebridge, bridge, };

struct ProcedureTemplate
{

	ProcedureType type {ProcedureType::any};
	int code{ 0 };
	std::string name;
	double price { 0 };
	
	std::string diagnosis;

	bool nzok{ false };

	//optional:
	std::string material;

};
