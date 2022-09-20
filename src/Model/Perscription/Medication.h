#pragma once
#include "Dosage.h"

#include <string>
#include <unordered_map>
#include <vector>

struct Medication
{
private:
	int key{ 0 }; //towa mai trqbwa da e key-q na map-a
	int form{ 0 }; //i tova sy6to, ama ot formite

public:

	enum Priority { Routine, Urgent, Asap, Stat};

	Medication() {};
	Medication(int key);

	Priority priority{ Routine };

	unsigned int quantity{ 1 };
	bool byForm{ false };
	bool substitution{ false };

	std::vector<Dosage> dosage;

	std::string note;

	bool setName(const std::string& name);

	int nhisPriority() { return priority + 1;}

	std::vector<std::string> dosageList() const;

	const std::string& name() const;
	const std::string& formStr() const;

	int getFormKey() { return form; };
	int getNumenclatureKey() { return key;}

	static bool isValidName(const std::string& name);
	static const std::unordered_map<std::string, int>& names();
	static const std::string& getFormByKey(int key);
	static const std::vector<std::string>& forms();

	static bool initialize();

};

