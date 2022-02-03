#pragma once
#include "Patient.h"
#include <vector>

struct ToothXML
{
	std::string toothCode;
	std::vector<std::string> toothStatus;
};

struct ServiceXML //a.k.a. procedure
{
	Date date;
	std::string diagnosis;
	int toothCode;
	int activityCode;
};

struct AmbListXML : public Patient
{
	std::string ambulatorySheetNo; //ambulatory list number (leading zeroes)
	std::string specificationType{ "PRIMARY NORM" };
	bool unfavourable_condition;
	int substitute{ 0 };
	int Sign{ 1 };
	std::vector<ToothXML> teeth;
	std::vector <ServiceXML> services;
};