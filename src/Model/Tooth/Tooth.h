#pragma once

#include "SurfStatus.h"
#include <string>
#include <array>

enum class StatusType{general, obturation, caries};

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
	
	void setIndex(int index);
	int index;

	ToothType type;

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


private:

	void addStatus(int statusCode);
	void removeStatus(int statusCode);
	void removeStatus();

public:

	std::array<bool, statusCount> getBoolStatus() const;
	void setStatus(StatusType type, int code, bool state = true);
	void setStatus(int code, bool state = true);
	void removeStatus(StatusType type);

	std::string getSimpleStatus() const;


};