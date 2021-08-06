#pragma once

#include "SurfStatus.h"
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

	SurfaceStatus<SurfaceChild<Obturation> > obturation;
	SurfaceStatus<SurfaceChild<Pathology> > caries;

	Status temporary;
	Status hyperdontic;
	Status periodontitis;
	Mobility mobility;

	DentistMade endo;
	Implant implant;
	DentistMade post;

	DentistMade extraction;

	Crown crown;
	Bridge bridge;

	Pathology pulpitis;
	Pathology lesion;
	Pathology fracture;
	Pathology root;

	std::array<bool, statusCount> getBoolArray() const;

	std::string getSimpleStatus() const;

};