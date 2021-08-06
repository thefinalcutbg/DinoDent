#pragma once
#include "user.h"

class CurrentUser
{
	inline static User _instance;

	CurrentUser() {};

public:
	CurrentUser(const CurrentUser&) = delete;

	static User& instance() { return _instance; }
};

