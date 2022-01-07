#pragma once
#include <string>
#include "Model/Date.h"

struct Practice
{
	std::string rziCode;
	std::string name;
	std::string bulstat;
	std::string contract;
	Date contractDate;
};

struct User
{
//	std::string username;
//std::string password;

	std::string name;

	std::string LPK;
	std::string practiceName;
	std::string RZI;

	int specialty{ -1 };

	std::string practice_address;


};

