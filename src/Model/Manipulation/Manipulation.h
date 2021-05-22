#pragma once
#include <string>
#include <array>
#include <variant>
#include "Model/Date.h"
#include "ManipulationTemplate.h"
#include "Model/Tooth/Vita.h"

struct NoData {};

struct BridgeData {

	int tooth_begin;
	int tooth_end;

	std::string material;
	int prep_type;
	Vita color;
};

struct CrownData {

	std::string material;
	int prep_type;
	Vita color;
};

struct ObtData{

	std::array<bool, 6>surfaces{ 0,0,0,0,0,0 };
	bool post;
	int color;
	std::string material;
};

struct ImplData {

};

typedef std::variant<NoData, ObtData, CrownData, BridgeData, ImplData> Result;

struct Manipulation
{


	Manipulation(const ManipulationTemplate& t, Date date, std::string name, std::string diagnosis, double price, Result result)
		:
		type{ t.type },
		code{ t.code },
		date{ date },
		name{ name },
		diagnosis{ diagnosis },
		price{ price },
		tooth{ -1 },
		nzok{ t.nzok },
		result{result}
	{}

	Manipulation() {}
	
	//common parameters:
	Date date;
	std::string diagnosis;
	int tooth;
	std::string name;
	int code;
	double price;

	std::string LPK;
	
	//status change parameters:
	ManipulationType type;

	Result result;

	//NZOK specific:
	bool nzok;

};




