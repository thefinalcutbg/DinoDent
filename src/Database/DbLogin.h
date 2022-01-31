#pragma once

#include "AbstractORM.h"
#include <vector>
#include "Model/User/User.h"
#include <optional>
#include <unordered_map>

struct PracticePair
{
	std::string rzi;
	std::string name;
};

class DbLogin : public AbstractORM
{
public:
	std::vector<PracticePair> getPracticeList(const std::string& doctorLPK);
	Practice getPractice(const std::string rziCode);
	std::optional<Doctor>getDoctor(const std::string& lpk, const std::string& pass);
	std::unordered_map<std::string, std::string> getDoctorNames(); //returns LPK - name as key-value
};

