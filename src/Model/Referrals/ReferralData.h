#pragma once
#include "RefNum.h"

struct MDD4Data
{
	int tooth{ -1 }; //-1 for OPG, any valid tooth idx for tooth xRay

	std::string getCode() { return tooth == -1 ? "10.62" : "06.01"; };
	std::string getKSMP() { return tooth == -1 ? "57960-00" : "57930-00"; };

};

struct R3Data
{
	static constexpr int specialty = 18; //Психиатрия
	static constexpr int hisSpecialty = 1050;

	std::string additionalDocuments;


};

struct R3AData
{
	static constexpr int nhifSpecialty = 26; //Анестезиология и интензивно лечение
	static constexpr int hisSpecialty = 1003; //според НЗИС
	static constexpr const char* highlySpecializedActivity{ "89.07" };
	static constexpr const char* ksmp{ "92500-00" };

	std::string additionalDocuments;
};


