#pragma once
#include <string>

struct User
{
	//std::string username;
//std::string password;

	std::string fName;
	std::string lName;

	std::string LPK;
	int specialty{ -1 };

	std::string practice_address;

	std::string getName() const {

		std::string name = u8"д-р ";
		name += fName;
		name += " ";
		name += lName;
		return name;
	};


};

