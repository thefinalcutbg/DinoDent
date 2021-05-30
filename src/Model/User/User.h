#pragma once
#include <string>

class User
{

	static User _instance;

	User() {};

public:
	User(const User&) = delete;

	std::string username;
	std::string password;

	std::string fName{ "Христо" };
	std::string lName{ "Константинов" };

	std::string LPK{"220008771"};
	int specialty{ 64 };

	std::string practice_address;

	static User& instance();

};

