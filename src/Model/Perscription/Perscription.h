#pragma once
#include <string>
#include "Model/Date.h"
#include <vector>
#include "Medication.h"

class Perscription
{

public:

	long long rowid;

	std::string LRN;
	Date date; //authoredOn

	static constexpr bool isProtocolBased = false; //protocol base, protocol num

	static constexpr const char* category = "T1";

	std::string basedOnNrn;

	static constexpr int financingSource = 4; //patient;

	enum DispensationType { SingleUse = 1, MultipleUses = 2 };
	DispensationType dispensationType;

	unsigned int numberOfRepeats{ 0 };//incase of MultipleUses

	std::string supplements;

	std::vector<Medication> medicationGroup;

};

