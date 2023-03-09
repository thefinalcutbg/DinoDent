#pragma once
#include "RefNum.h"

struct MDD4Data
{
	
private:

	static inline std::string 
		opgNhif = "10.62", 
		opgKsmp = "57960-00",
		segmentNhif = "06.01",
		segmentKsmp = "57930-00";

public:
	
	int tooth_idx{ -1 }; //-1 for OPG, any valid tooth idx for tooth xRay
	inline bool isOPG() const { return tooth_idx == -1; }
	inline const std::string& getCode() const { return isOPG() ? opgNhif : segmentNhif; };
	inline const std::string& getKSMP() const { return isOPG() ? opgKsmp : segmentKsmp; };

};

struct R3Data
{
	static constexpr int specialty = 18; //Психиатрия
	static constexpr int hisSpecialty = 1050;

};

struct R3AData
{
	static constexpr int nhifSpecialty = 26; //Анестезиология и интензивно лечение
	static constexpr int hisSpecialty = 1003; //според НЗИС
	static constexpr const char* highlySpecializedActivity{ "89.07" };
	static constexpr const char* ksmp{ "92500-00" };

};


struct MH119Data
{
	MH119Spec specCode{ 0 };
	MH119Reason reason{ 0 };
	std::string description;

	int getSpecCode() const {
		static constexpr int codes[3]{ 61, 62, 64 };
		return codes[specCode.getIndex()];
	}
};