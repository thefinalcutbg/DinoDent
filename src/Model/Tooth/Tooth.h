#pragma once

#include "Surfaces.h"
#include <string>
#include <array>

class Tooth
{
	static std::string cariesDefault;
	static std::string pulpitisDefault;
	static std::string lesionDefault;
	static std::string fractureDefault;
	static std::string rootDefault;
	static std::string obturationDefault;
	static std::string prosthoDefault;

public:
	Tooth();

	int index;

	bool obturation;
	bool caries;

	std::array <DentistMade, 6> o_surf;
	std::array <Pathology, 6> c_surf;

	SurfaceTemplate<DentistMade> obtur;
	SurfaceTemplate<Pathology> car;

	Status temporary;
	Status hyperdontic;
	Status periodontitis;
	Mobility mobility;


	DentistMade endo;
	DentistMade implant;
	DentistMade post;
	DentistMade extraction;
	DentistMade crown;
	Bridge bridge;

	Pathology pulpitis;
	Pathology lesion;
	Pathology fracture;
	Pathology root;
};