#pragma once

#include "SurfStatus.h"
#include <string>
#include <array>
#include "DiagnosisContainer.h"

enum class StatusType{general, obturation, caries};

class Tooth
{

public:
	Tooth();
	
	void setIndex(int index);
	int index;

	ToothType type;

	SurfaceStatus<SurfaceChild<Obturation> > obturation;
	SurfaceStatus<SurfaceChild<Caries> > caries;

	Status temporary;
	Status hyperdontic;
	Status periodontitis;
	Status impacted;
	
	DentistMade endo;
	
	DentistMade extraction;
	
	Implant implant;
	Crown crown;
	Bridge bridge;
	FiberSplint splint;

	Pulpitis pulpitis;
	ApicalLesion lesion;
	Fracture fracture;
	Root root;
	Post post;

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

	//for printing (T status is omitted)
	std::string getStringStatus() const;
	//for xml report generation
	std::vector<std::string> getSimpleStatuses() const;


};