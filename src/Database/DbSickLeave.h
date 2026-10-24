#pragma once
#include "Model/SickLeave.h"

class Db;

namespace DbSickLeave 
{

	long long getNumber();
	long long getFreeNumberCount();
	std::vector<SickLeave> getSickLeave(long long ambListId, Db& db);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId, Db& db);

}