#pragma once
#include <string>
#include <vector>
#include "Model/Date.h"
#include "Model/Procedure/ProcedureTemplate.h"

struct Doctor
{
	std::string LPK;
	std::string pass;
	std::string doctor_name;
	int specialty{ -1 };
};


struct Practice
{
	std::string rziCode;
	std::string practice_name;
	std::string bulstat;
	std::string contract;
	Date contractDate;
	std::string practice_address;
	std::vector<ProcedureTemplate> priceList;
};

struct User : public Doctor, public Practice {};

