#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "UserStructs.h"

/*
struct Practice;
struct Doctor;
struct User;
struct Settings;
*/

namespace User
{
	void initialize();

	const Practice& practice();
	const Settings& settings();
	const Doctor& doctor();

	void setCurrentDoctor(const Doctor& doctor);
	void setPriceList(const std::vector<ProcedureTemplate>& priceList);
	void setCurrentPractice(const Practice& practice);
	void resetUser();
	const bool isCurrentUser(const std::string& LPK);
	bool isAdmin();
	void refereshPracticeDoctor();
	std::string getNameFromLPK(const std::string& LPK);
	double getPrice(int procedureCode);
	bool hasNzokContract();
};

