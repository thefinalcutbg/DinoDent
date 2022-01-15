#pragma once

#include "AbstractORM.h"
#include <vector>
#include "Model/User/User.h"
#include <optional>
#include <unordered_map>

class DbLogin : public AbstractORM
{
public:
	std::vector<Practice> practiceList();
	std::optional<Doctor>getDoctor(const std::string& lpk, const std::string& pass, const std::string& rziCode);
	std::unordered_map<std::string, std::string> getDoctorNames(); //returns LPK - name as key-value
};

