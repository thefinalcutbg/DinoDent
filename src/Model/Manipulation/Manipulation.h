#pragma once
#include <string>
#include <array>
#include <variant>
#include "Model/Date.h"
#include "ManipulationTemplate.h"

struct BridgeParam {
	int tooth_begin;
	int tooth_end;
};

struct ObtParam{
	std::array<bool, 6>surfaces;
};

struct Manipulation
{
	Manipulation(const ManipulationTemplate& t, Date date, std::string name, std::string diagnosis, double price)
		:
		type{ t.type },
		code{ t.code },
		date{ date },
		name{ name },
		diagnosis{ diagnosis },
		price{ price },
		tooth{ -1 },
		nzok{ t.nzok },
		duration{ t.duration },
		nzok_price{t.nzok_price}
	{}


	
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
	std::string material; //<-should go in the respective parameters
	std::variant<ObtParam, BridgeParam> result;

	//NZOK specific:
	bool nzok;
	int duration;
	double nzok_price;

};




