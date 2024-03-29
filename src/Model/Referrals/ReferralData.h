﻿#pragma once
#include "RefNum.h"

struct MDD4Data
{
	
private:

	static inline std::string
		opgNhif = "10.62",
		opgKsmp = "57960-00",
		opgHis = "0C-010",
		segmentNhif = "06.01",
		segmentKsmp = "57930-00",
		segmentHis = "0C-00E";

public:
    MDD4Data(int tooth_idx = -1) : tooth_idx(tooth_idx) {}
	int tooth_idx{ -1 }; //-1 for OPG, any valid tooth idx for tooth xRay
	inline bool isOPG() const { return tooth_idx == -1; }
	inline const std::string& getCode() const { return isOPG() ? opgNhif : segmentNhif; };
	inline const std::string& getKSMP() const { return isOPG() ? opgKsmp : segmentKsmp; };
	inline const std::string& getHisCode() const{ return isOPG() ? opgHis : segmentHis; };
};

struct R3Data
{
	static constexpr const char* specialty = "18"; //Психиатрия
	static constexpr const char* hisSpecialty = "1050";

};

struct R3AData
{
	static constexpr const char* nhifSpecialty = "26"; //Анестезиология и интензивно лечение
	static constexpr const char* hisSpecialty = "1003"; //според НЗИС
	static constexpr const char* highlySpecializedActivity{ "89.07" };
	static constexpr const char* ksmp{ "92500-00" };

};


struct MH119Data
{
	enum MH119Spec { Pediatric, Surgery };

	MH119Spec specCode{ Surgery };
	MH119Reason reason{ 0 };
	std::string description;
};
