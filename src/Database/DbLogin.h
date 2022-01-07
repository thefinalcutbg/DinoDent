#pragma once

#include "AbstractORM.h"
#include <vector>
#include "Model/User/User.h"
#include <optional>

class DbLogin : public AbstractORM
{
public:
	std::vector<Practice> practiceList();
	std::optional<User>getUser(const std::string& lpk, const std::string& pass, const std::string& rziCode);
};

