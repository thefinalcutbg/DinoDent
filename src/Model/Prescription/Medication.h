#pragma once
#include "Dosage.h"
#include "Model/Date.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct DosePeriod
{
	Date from = Date::currentDate();
	Date to = Date::currentDate();
};

struct Medication
{
private:
	int key{ 0 }; //medication key
	int form{ 0 }; //medication form

	static inline std::unordered_map<int, std::pair<std::string, int>> s_medications;
	static inline std::unordered_map<std::string, int> s_medNameToIdx;
	static inline std::vector<std::string> s_medForms;
	static inline std::string s_dummyResult{};

public:

	enum Priority { Routine, Urgent, Asap, Stat};

	Medication() {};
	Medication(int key);

	Priority priority{ Routine };

	unsigned int quantity{ 1 };
	bool byForm{ false };
	bool substitution{ false };

	std::vector<Dosage> dosage;

	std::optional<DosePeriod> dosePeriod;

	std::string note;

	bool setName(const std::string& name);

	int nhisPriority() { return priority + 1;}

	std::vector<std::string> dosageList() const;

	const std::string& name() const;
	const std::string& formStr() const;

	int getFormKey() const { return form; };
	int getNumenclatureKey() const { return key;}

	static bool isValidName(const std::string& name);
	static const std::unordered_map<std::string, int>& names();
	static const std::string& getFormByKey(int key);
	static const std::vector<std::string>& forms();
	static bool initialize();

};

