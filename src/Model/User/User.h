#pragma once
#include <string>

struct User
{
	//std::string username;
//std::string password;

	std::string fName{ u8"Христо" };
	std::string lName{ u8"Константинов" };

	std::string LPK{ "220008771" };
	int specialty{ 64 };

	std::string practice_address{ u8"София, общ. Столична, обл. София-град" };

	std::string getName() const;
};

