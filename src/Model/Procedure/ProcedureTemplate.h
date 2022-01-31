#pragma once
#include <string>

enum class ProcedureType 
{ 
	general = 0, 
	any = 1, 
	obturation = 2, 
	extraction = 3, 
	endo = 4, 
	crown = 5, 
	implant = 6, 
	bridge = 7, 
	fibersplint = 8, 
	removecrown = 9, 
	removebridge = 10,  
};

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
