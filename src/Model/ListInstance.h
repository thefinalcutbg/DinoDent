#pragma once

#include <vector>

#include "Patient.h"
#include "AmbList.h"
#include "string"

struct ListInstance
{
	bool edited;

	Patient patient;
	AmbList amb_list;

	ListInstance(AmbList ambList, Patient patient);
	bool isNew();
	bool isEdited();
	std::vector <int> selectedIndexes;

	std::string getTabName();

	
};