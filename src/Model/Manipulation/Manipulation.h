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
		type {t.type},
		code {t.code},
		duration {t.duration},
		date {date},
		name {name},
		diagnosis{ diagnosis },
		price {price},
		tooth { -1 }

	{}


	ManipulationType type;
	Date date;
	int code;
	std::string name;
	int tooth;
	std::string diagnosis;
	double price;
	int duration;
	std::string LPK;

	std::string material;

	std::variant<ObtParam, BridgeParam> additional;

};




