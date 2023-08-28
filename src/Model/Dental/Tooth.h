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
	Tooth(int index);
	Tooth(const Tooth& other);
	Tooth& operator=(const Tooth& t);
	//call if the tooth can have supernumeral twin

	std::array<bool, statusCount> getBoolStatus() const;
	void addStatus(int statusCode);
	void removeStatus(int statusCode);
	void removeStatus();
	void setStatus(StatusType type, int code, bool state = true);
	void setStatus(int code, bool state = true);
	void removeStatus(StatusType type);

	bool isPontic() const;
	bool canHaveACrown() const;
	//for printing (T status is omitted)
	std::string getPrintStatus() const;
	//for xml report generation
	std::vector<std::string> getSimpleStatuses() const;
	std::vector<std::string> getHISStatus() const;
	std::string getToothInfo() const;
	std::string toothName() const;
	bool noData() const;

	const int index;
	const ToothType type;

	SurfaceStatus<SurfaceChild<DentistMade> > obturation;
	SurfaceStatus<SurfaceChild<Status> > caries;

	Status healthy;
	Status temporary;
	Dsn dsn;
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

};