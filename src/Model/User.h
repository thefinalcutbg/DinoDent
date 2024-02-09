#pragma once

#include <string>
#include "UserStructs.h"

struct Issuer;

namespace User
{
	void initialize();

	const Practice& practice();
	const Settings& settings();
	const Doctor& doctor();
	void setCurrentDoctor(const Doctor& doctor);
	void setCurrentPractice(const Practice& practice);
	void resetUser();
    bool isCurrentUser(const std::string& LPK);
	bool isAdmin();
	void refereshPracticeDoctor();
	std::string getNameFromLPK(const std::string& LPK);
//	double getPrice(int procedureCode);
	bool hasNhifContract();
};

