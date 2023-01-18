#pragma once
#include <string>

enum class ProcedureTemplateType
{
	general = 0,
	any = 1,
	obturation = 2,
	extraction = 3,
	endodontic = 4,
	prosthodontic = 5,
	implant = 6,
	fibersplint = 7,
	removecrown = 8,
	removepost = 9,
	removebridgeorsplint = 10
};

struct ProcedureTemplate
{

	ProcedureTemplateType type { ProcedureTemplateType::any};
	int code{ 0 };
	std::string name;
	double price { 0 };
	
	std::string diagnosis;

	bool nhif{ false };

	//std::string material;
	std::string ksmp;

};
