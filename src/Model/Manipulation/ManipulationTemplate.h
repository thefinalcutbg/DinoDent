#pragma once
#include <string>

enum class ManipulationType { general, any, obturation, extraction, endo, crown, bridge, implant};


struct ManipulationTemplate
{

	ManipulationType type {ManipulationType::any};
	int code{ 0 };
	std::string name;
	double price { 0 };
	
	std::string diagnosis;

	bool nzok{ false };
	int duration{ 0 };
	double nzok_price{ 0 };


	//optional:
	std::string material;

};
