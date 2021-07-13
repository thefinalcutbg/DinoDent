#pragma once
#include <string>

class CurrentUser
{

	static CurrentUser _instance;

	CurrentUser() {};

public:
	CurrentUser(const CurrentUser&) = delete;

	//std::string username;
	//std::string password;

	std::string fName{ "Христо" };
	std::string lName{ "Константинов" };

	std::string LPK{"220008771"};
	int specialty{ 64 };

	std::string practice_address{ "София, общ. Столична, обл. София-град" };

	static CurrentUser& instance();

};

