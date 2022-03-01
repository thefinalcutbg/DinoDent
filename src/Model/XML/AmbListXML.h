#pragma once
#include "Model/Patient.h"
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


struct AmbListXML
{
	std::string id; //needed for mapping services
	int personType;
	std::string personIdentifier;
	std::string RHIF;
	std::string HealthRegion;
	std::string personFirstName;
	std::string personMiddleName;
	std::string personLastName;
	std::string specificationType;
	int ambulatorySheetNo; //six digit number with leading zeroes
	std::string HIRBNo;
	std::string allergies;
	std::string pastDiseases;
	std::string currentDiseases;
	Date birthDate;
	int substitute{ 0 };
	int sign{ 1 };



	std::vector<ToothXML> teeth;
	std::vector <ServiceXML> services;
};