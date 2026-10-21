#pragma once
#include "Model/SickLeave.h"

class Db;

namespace DbSickLeave 
{
	// First unused number in the current practice range; empty if unavailable.
	// Does not reserve the number; persist it before requesting another.
	long long getNumber();
	std::vector<SickLeave> getSickLeave(long long ambListId, Db& db);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId);
	void saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId, Db& db);

}