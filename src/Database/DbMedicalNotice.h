#pragma once
#include <vector>
#include "Model/MedicalNotice.h"

class Db;

namespace DbMedicalNotice
{
	std::vector<MedicalNotice> get(long long amblistRowid, Db& db);
	std::vector<MedicalNotice> get(long long amblistRowid);
	void save (const std::vector<MedicalNotice> notices, long long amblistRowid, Db& db);
	void save (const std::vector<MedicalNotice> notices, long long amblistRowid);
}