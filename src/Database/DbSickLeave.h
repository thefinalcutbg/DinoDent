#pragma once
#include "Model/SickLeave.h"

class Db;

namespace DbSickLeave 
{
	std::string getNumber();
	std::vector<SickLeave> getSickLeave(long long ambListId, Db& db);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId, Db& db);

}