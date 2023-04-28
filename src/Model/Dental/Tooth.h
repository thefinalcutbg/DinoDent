#pragma once

#include "SurfStatus.h"
#include <string>
#include <array>
#include "Model/FreeFunctions.h"

enum class StatusType{general, obturation, caries, mobility};

class Tooth
{

	bool isHealthyCheck();

public:
	Tooth();
	
	void setIndex(int index);
	int index;

	ToothType type;

	SurfaceStatus<SurfaceChild<DentistMade> > obturation;
	SurfaceStatus<SurfaceChild<Status> > caries;

	Status healthy;
	Status temporary;
	Status hyperdontic;
	Status periodontitis;
	Status impacted;
	Status calculus;
	DentistMade endo;
	
	DentistMade extraction;
	
	DentistMade implant;
	DentistMade crown;
	DentistMade denture;
	Construction bridge;
	Construction splint;

	Status pulpitis;
	Status lesion;
	Status fracture;
	Status root;
	DentistMade post;

	Mobility mobility;


public:

	std::array<bool, statusCount> getBoolStatus() const;
	void addStatus(int statusCode);
	void removeStatus(int statusCode);
	void removeStatus();
	void setStatus(StatusType type, int code, bool state = true);
	void setStatus(int code, bool state = true);
	void removeStatus(StatusType type);
	std::string getToothInfo() const;
	std::string toothName() const;
	bool canHaveACrown() const;
	//for printing (T status is omitted)
	std::string getStringStatus() const;
	//for xml report generation
	std::vector<std::string> getSimpleStatuses() const;
	bool noData() const;
};