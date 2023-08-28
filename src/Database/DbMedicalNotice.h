#pragma once
#include <vector>
#include "Model/MedicalNotice.h"

class Db;

namespace DbMedicalNotice
{
	std::vector<MedicalNotice> get(long long amblistRowid);
	void save (const std::vector<MedicalNotice> notice, long long amblistRowid, Db& db);
	void save (const std::vector<MedicalNotice> notice, long long amblistRowid);
}