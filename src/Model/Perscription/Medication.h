#pragma once
#include "Dosage.h"

#include <string>
#include <unordered_map>
#include <vector>

struct Medication
{
	
	int key; //towa mai trqbwa da e key-q na map-a
	int form; //i tova sy6to, ama ot formite

	static constexpr int priority = 1;
	std::string note;

	unsigned int quantityValue{ 1 };
	bool isQuantityByForm{ false };
	bool isSubstitutionAllowed{ false };

	std::vector<Dosage> dosage;

	bool setName(const std::string& name);

	std::vector<std::string> dosageList() const;

	const std::string& name() const;
	const std::string& formStr() const;

	static bool isValidName(const std::string& name);
	static const std::unordered_map<std::string, int>& names();
	static const std::string& getFormByKey(int key);
	static const std::vector<std::string>& forms();

	static bool initialize();

};

