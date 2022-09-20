#pragma once
#include <string>
#include "Model/Date.h"
#include <vector>
#include "Medication.h"
#include "Dispensation.h"

struct Prescription
{

	long long rowid{ 0 };
	long long patient_rowid{ 0 };

	std::string LRN;
	std::string NRN;

	Date date{ Date::currentDate() };//authoredOn

	static constexpr bool isProtocolBased = false; //protocol base, protocol num

	static constexpr const char* category = "T1";

	static constexpr int financingSource = 4; //patient;

	std::string basedOnNrn;

	Dispensation dispensation;

	std::string supplements;

	std::vector<Medication> medicationGroup;

	std::vector<std::string> getMedList() const;

};

