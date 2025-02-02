#pragma once

#include <string>
#include "UserStructs.h"
#include "Printer/SignatureTablet.h"

struct Issuer;

namespace User
{
	void initialize();

	bool isValid();
	const Practice& practice();
	const Settings& settings();
	const Doctor& doctor();
	SignatureTablet& signatureTablet();
	void setCurrentDoctor(const Doctor& doctor);
	void setCurrentPractice(const Practice& practice);
	void resetUser();
    bool isCurrentUser(const std::string& LPK);
	bool isAdmin();
	void refereshPracticeDoctor();
	std::string getNameFromLPK(const std::string& LPK);
//	double getPrice(int procedureCode);
	bool hasNhifContract();
	bool isIncognito();
	void setIncognito(bool incognito);
};

