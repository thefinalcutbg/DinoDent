#pragma once

#include <vector>
#include <memory>
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "string"
#include "Model/AmbListValidator.h"

struct ListInstance
{

	bool edited;
	int _scrollWidth{ 0 };
	int _scrollHeight{ 0 };

	std::shared_ptr<Patient> patient;
	AmbList amb_list{};
	
	ListInstance();
	ListInstance(const ListInstance& old_obj);
	ListInstance(AmbList ambList, std::shared_ptr<Patient> patient);
	bool isNew();
	bool isEdited();


	std::vector <Tooth*> selectedTeeth;

	std::string getTabName();
	
};