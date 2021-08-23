#pragma once

#include <vector>
#include <memory>
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "string"
#include "Model/AmbListValidator.h"

struct ListInstance
{
	AmbList amb_list;
	std::shared_ptr<Patient> patient;
	bool edited;
	int _scrollWidth{ 0 };
	int _scrollHeight{ 0 };


	
	ListInstance();
	ListInstance(const ListInstance& old_obj);
	ListInstance(const AmbList &ambList, std::shared_ptr<Patient> patient);
	bool isNew();
	bool isEdited();


	std::vector <Tooth*> selectedTeeth;

	std::string getTabName();
	
};