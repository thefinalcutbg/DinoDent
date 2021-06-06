#pragma once

#include <vector>
#include <memory>
#include "Patient.h"
#include "AmbList.h"
#include "string"

struct ListInstance
{
	bool edited;

	std::shared_ptr<Patient> patient;
	AmbList amb_list;
	

	ListInstance(AmbList ambList, std::shared_ptr<Patient> patient);
	bool isNew();
	bool isEdited();
//	std::vector <int> selectedIndexes;
	std::vector <Tooth*> selectedTeeth;

	std::string getTabName();

	
};