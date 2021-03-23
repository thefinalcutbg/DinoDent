#pragma once
#include <string>

enum class ManipulationType { general, obturation, extraction, endo, crown, bridge, any };


struct ManipulationTemplate
{
	ManipulationType type;
	int code;
	std::string name;
	double price;
	int duration;

	//optional
	std::string diagnosis;
	std::string material;

};
